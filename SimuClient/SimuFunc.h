#ifndef SIMU_FUNC_H
#define SIMU_FUNC_H

#include "../AdminServer/admin_client.h"
#include "../Comp/comp_client.h"

#include "ReqAnalytics.h"
#include "ServiceSelector.h"
#include "phxrpc/network/timer.h"
#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <random>
#include <signal.h>    /* union sigval / struct sigevent */
#include <unistd.h> /* sleep */

using namespace std;

AdminClient * g_adminProxy;
map<string, ServiceSelector> * g_serviceTable;
map<uint32_t, ReqLog> g_rstData;

bool TestAccessAdminServer()
{
	google::protobuf::StringValue req;
	google::protobuf::StringValue resp;
	req.set_value("Access AdminServer Success");
	int32_t ret = g_adminProxy->PhxEcho(req, &resp);
	printf("AdminServer.PhxEcho return %d\n", ret);
	printf("resp: {\n%s}\n", resp.DebugString().c_str());
	return ret == 0;
}

bool ReadTrafficFile(string filePath, vector<AppReq> & traffic)
{
	FILE * f = fopen(filePath.c_str(), "rb");
	if (f == NULL)
	{
		return false;
	}
	uint32_t n = 0;
	fread(&n, sizeof(n), 1, f);
	for (uint32_t i = 0; i < n; ++i)
	{
		AppReq req;
		fread(&req, sizeof(req), 1, f);
		traffic.push_back(req);
	}
	fclose(f);
	return true;
}

int UpdateServiceTable(map<string, ServiceSelector> & table)
{
	magna::ServiceTableRequest req;
	magna::ServiceTableResponse rsp;
	int ret = g_adminProxy->GetServiceTable(req, &rsp);
	if (ret != 0)
	{
		return -1;
	}

	table.clear();
	phxrpc::Endpoint_t ep;
	for (int32_t i = 0; i < rsp.routertable().size(); ++i)
	{
		string name = rsp.routertable(i).name();
		memset(&ep, 0, sizeof(ep));
		strcpy(ep.ip, rsp.routertable(i).ep().ip().c_str());
		ep.port = rsp.routertable(i).ep().port();
		double percentage = rsp.routertable(i).percentage();
		table[name].AddService(ep, percentage);
	}
	return 0;
}

int StartSimu(const vector<AppReq> & traffic, map<uint32_t, ReqLog> & rstData, map<int32_t, int32_t> & retMap)
{
	magna::AppRequest simuReq;
	magna::AppResponse simuRsp;
	uint32_t i = -1;
	static uint32_t sendCount = 0;
	
	uint64_t stamp = phxrpc::Timer::GetSteadyClockMS();
	while (++i < traffic.size())
	{
		usleep(traffic[i].interval);
		simuReq.set_id(traffic[i].id);
		simuReq.set_clienttype(traffic[i].weight);
		string serviceName = traffic[i].service;
		simuReq.set_servicename(serviceName);

		ReqLog reqLog;
		reqLog.reqId = traffic[i].id;
		strcpy(reqLog.serviceName, traffic[i].service);
		reqLog.clientWeight = traffic[i].weight;
		reqLog.localBegin = phxrpc::Timer::GetSteadyClockMS();
		map<string, ServiceSelector>::iterator foundIt = g_serviceTable->find(serviceName);
		if (foundIt == g_serviceTable->end())
		{
			cout << "Service not found\n";
			continue;// ��Ҫ���ؿ����Ƿ����ֱ��������������
		}

		CompClient cc;

		cout << "sendcount: " << ++sendCount << endl;
		int32_t ret = cc.Handle(foundIt->second.GetService(), simuReq, &simuRsp);
		if (ret == 0)
		{
			// reqLog.localEnd = phxrpc::Timer::GetSteadyClockMS();
			rstData[reqLog.reqId] = reqLog;
		}
		else
		{
			++retMap[ret];
		}
	}
	// ������߲���
// 	int32_t stressTmp = 0;
// 	map<string, ServiceSelector>::iterator foundIt = g_serviceTable->begin();
// 	while (++stressTmp < 100000)
// 	{
// 		CompClient cc;
// 		int32_t ret = cc.Handle(g_serviceTable->begin()->second.GetService(), simuReq, &simuRsp);
// 		if (ret == 0)
// 		{
// 			cout << "sendcount: " << ++sendCount << endl;
// 		}
//	}
	cout << "Time Used: " << phxrpc::Timer::GetSteadyClockMS() - stamp << "ms\n";

	uint64_t tmp = 0;
	for (uint32_t i = 0; i < traffic.size(); ++i)
	{
		tmp += traffic[i].interval;
	}
	cout << "Time expect: " << tmp / 1000 << "ms\n";

	
	return 0;
}

int SimuAll()
{
	AdminClient::Init("../AdminServer/admin_client.conf");
	CompClient::Init("../Comp/comp_client.conf");

	g_adminProxy = new AdminClient;

	// �����Ƿ���Է���AdminServer
	if (!TestAccessAdminServer())
	{
		cout << "AdminServer not available\n";
		return -1;
	}

	// �������������
	string dataFile = "../TrafficGenerator/test.dat";
	vector<AppReq> traffic;
	if (!ReadTrafficFile(dataFile, traffic))
	{
		cout << "Traffic file read error\n";
		return -2;
	}

	// ��ȡ����·�ɱ�
	map<string, ServiceSelector> serviceTable;
	if (UpdateServiceTable(serviceTable) < 0)
	{
		cout << "Fail to get ServiceTable\n";
		return -3;
	}
	g_serviceTable = &serviceTable;

	// ��AdminServer�����������󣬲���¼��ʼ�ͽ�ֹ��ʱ���
	
	map<int32_t, int32_t> retMap;

	StartSimu(traffic, g_rstData, retMap);


	return 0;
}


int Stress(string compName, const phxrpc::Endpoint_t & ep, map<int, string> & trafficFiles)
{
	CompClient::Init("../Comp/comp_client.conf");

	// ��ȡ����·�ɱ�
	ServiceSelector selector;
	map<string, ServiceSelector> serviceTable;
	selector.AddService(ep, 1);
	serviceTable.insert(make_pair(compName, selector));
	g_serviceTable = &serviceTable;

	map<int32_t, int32_t> retMap;
	for (map<int, string>::iterator it = trafficFiles.begin(); it != trafficFiles.end(); it++)
	{
		int lamda = it->first;
		// �������������
		string dataFile = it->second;
		vector<AppReq> traffic;
		if (!ReadTrafficFile(dataFile, traffic))
		{
			cout << "Traffic file read error\n";
			return -2;
		}
		
		// ��ʼѹ��
		
		StartSimu(traffic, g_rstData, retMap);
	}
	
	return 0;
}




#endif//SIMU_FUNC_H