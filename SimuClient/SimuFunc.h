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
#include "Semaphore.h"

using namespace std;

AdminClient * g_adminProxy;
map<string, ServiceSelector> * g_serviceTable;

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

int StartSimu(const vector<AppReq> & traffic, vector<ReqLog> & rstData, map<int32_t, int32_t> & retMap)
{
	magna::AppRequest simuReq;
	magna::AppResponse simuRsp;
	uint32_t index = -1;
	queue<uint32_t> waitQueue;
	Semaphore sema(0);
	bool shouldRun = true;
	uint32_t sendCount = 0;
	auto func = [&]()
	{
		while (shouldRun)
		{
			sema.wait();

			uint32_t i = waitQueue.front();
			waitQueue.pop();

			simuReq.set_id(traffic[i].id);
			simuReq.set_clienttype(traffic[i].weight);
			string serviceName = traffic[i].service;
			simuReq.set_servicename(serviceName);

			ReqLog reqLog;
			reqLog.req = traffic[i];

			reqLog.begin = phxrpc::Timer::GetSteadyClockMS();
			map<string, ServiceSelector>::iterator foundIt = g_serviceTable->find(serviceName);
			if (foundIt == g_serviceTable->end())
			{
				cout << "Service not found\n";
				return;// ��Ҫ���ؿ����Ƿ����ֱ��������������
			}

			CompClient cc;

			cout << "sendcount: " << ++sendCount << endl;
			int32_t ret = cc.Handle(foundIt->second.GetService(), simuReq, &simuRsp);
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

	};
	std::thread sendThread(func);

	uint64_t stamp = phxrpc::Timer::GetSteadyClockMS();
	while (++index < traffic.size())
	{
		usleep(traffic[index].interval);
		waitQueue.push(index);
		sema.signal();
	}

	cout << "Time Used: " << phxrpc::Timer::GetSteadyClockMS() - stamp << "ms\n";

	uint64_t tmp = 0;
	for (uint32_t i = 0; i < traffic.size(); ++i)
	{
		tmp += traffic[i].interval;
	}
	cout << "Time expect: " << tmp / 1000 << "ms\n";
	while (true)
	{
		sleep(1);
		if (waitQueue.empty())
		{
			shouldRun = false;
			break;
		}
	}
	
	sendThread.join();
	return 0;
}



#endif//SIMU_FUNC_H
