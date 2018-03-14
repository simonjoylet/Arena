#include "../AdminServer/admin_client.h"
#include "../Comp/comp_client.h"

// #include <google/protobuf/stubs/common.h>
// using google::protobuf::int32;
// using google::protobuf::uint32;

#include "ReqAnalytics.h"
#include "phxrpc/network/timer.h"
#include <iostream>
#include <map>
#include <vector>
using namespace std;

AdminClient * g_adminProxy;
map<string, vector<pair<phxrpc::Endpoint_t, double>>> g_serviceTable;

bool TestAccessAdminServer();

bool ReadTrafficFile(string filePath, vector<AppReq> & traffic);

int UpdateServiceTable(map<string, vector<pair<phxrpc::Endpoint_t, double>>> & table);

int StartSimu(
	const vector<AppReq> & traffic, 
	vector<ReqLog> & rstData, 
	map<int32_t, int32_t> & retMap);

int main()
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
	if (UpdateServiceTable(g_serviceTable) < 0)
	{
		cout << "Fail to get ServiceTable\n";
		return -3;
	}
	

	// ��AdminServer�����������󣬲���¼��ʼ�ͽ�ֹ��ʱ���
	vector<ReqLog> rstData;
	map<int32_t, int32_t> retMap;
	uint64_t stamp = phxrpc::Timer::GetSteadyClockMS();
	StartSimu(traffic, rstData, retMap);
	cout << "Time Used: " << phxrpc::Timer::GetSteadyClockMS() - stamp << "ms\n";
	return 0;
}












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

int StartSimu(const vector<AppReq> & traffic, vector<ReqLog> & rstData, map<int32_t, int32_t> & retMap)
{
	magna::AppRequest simuReq;
	magna::AppResponse simuRsp;
	for (uint32_t i = 0; i < traffic.size(); ++i)
	{
		simuReq.set_id(traffic[i].id);
		simuReq.set_clienttype(traffic[i].weight);
		simuReq.set_servicename(traffic[i].service);

		ReqLog reqLog;
		reqLog.req = traffic[i];

		reqLog.begin = phxrpc::Timer::GetSteadyClockMS();
		if (g_serviceTable.find(traffic[i].service) == g_serviceTable.end())
		{
			cout << "Service not found\n";
			continue;// ��Ҫ���ؿ����Ƿ����ֱ��������������
		}
		
		vector<pair<phxrpc::Endpoint_t, double>> & balancer = g_serviceTable[traffic[i].service];
		//������ʹ���Ժ���д�������ƻ���vector�洢�ۼƸ��ʣ�Ȼ���䵽�ĸ����䡣

		CompClient cc;
		int32_t ret = cc.Handle(balancer[0].first, simuReq, &simuRsp);
		
		//int32_t ret = g_adminProxy->Handle(simuReq, &simuRsp);

		if (ret == 0)
		{
			reqLog.end = phxrpc::Timer::GetSteadyClockMS();
			rstData.push_back(reqLog);
		}
		else
		{
			++retMap[ret];
		}

	}

	return 0;
}

int UpdateServiceTable(map<string, vector<pair<phxrpc::Endpoint_t, double>>> & table)
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
		table[name].push_back(make_pair(ep, percentage));
	}
	return 0;
}

