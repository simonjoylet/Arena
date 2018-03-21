#include "SimuFunc.h"

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

int UpdateServiceTable()
{
	magna::ServiceTableRequest req;
	magna::ServiceTableResponse rsp;
	while (true)
	{
		int ret = g_adminProxy->GetServiceTable(req, &rsp);
		if (ret != 0)
		{
			printf("Get service table failed, ret: %d\n", ret);
			continue;
		}
		g_routerMutex.lock();
		g_serviceTable.clear();
		phxrpc::Endpoint_t ep;
		for (int32_t i = 0; i < rsp.routertable().size(); ++i)
		{
			string name = rsp.routertable(i).name();
			memset(&ep, 0, sizeof(ep));
			strcpy(ep.ip, rsp.routertable(i).ep().ip().c_str());
			ep.port = rsp.routertable(i).ep().port();
			double percentage = rsp.routertable(i).percentage();
			g_serviceTable[name].AddService(ep, percentage);
		}
		g_routerMutex.unlock();
		
		sleep(1);
	}
	
	
	return 0;
}

int StartSimu(const vector<AppReq> & traffic, map<uint32_t, ReqLog> & rstData, map<int32_t, int32_t> & retMap)
{
	magna::AppRequest simuReq;
	magna::AppResponse simuRsp;
	uint32_t i = -1;
	
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

		// ����·�ɱ�
		g_routerMutex.lock();
		map<string, ServiceSelector>::iterator foundIt = g_serviceTable.find(serviceName);
		if (foundIt == g_serviceTable.end())
		{
			cout << "Service not found\n";
			g_routerMutex.unlock();
			continue;// ��Ҫ���ؿ����Ƿ����ֱ��������������
		}
		ServiceSelector selector = foundIt->second;
		g_routerMutex.unlock();

		CompClient cc;

		cout << "sendcount: " << ++g_sendCount << endl;
		int32_t ret = cc.Handle(selector.GetService(), simuReq, &simuRsp);
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

	cout << "Time Used: " << phxrpc::Timer::GetSteadyClockMS() - stamp << "ms\n";

	uint64_t tmp = 0;
	for (uint32_t i = 0; i < traffic.size(); ++i)
	{
		tmp += traffic[i].interval;
	}
	cout << "Time expect: " << tmp / 1000 << "ms\n";


	return 0;
}

int SaveLogData(string filePath)
{
	FILE * rstFile = fopen(filePath.c_str(), "wb");
	if (rstFile == NULL)
	{
		printf("File open failed, path: %s\n", filePath.c_str());
		return -1;
	}

	// ���渺������
	g_loadLogDataMutex.lock();
	uint32_t loadLogCount = g_loadLogList.size();
	fwrite(&loadLogCount, sizeof(loadLogCount), 1, rstFile);
	for (size_t i = 0; i < loadLogCount; i++)
	{
		LoadLog & log = g_loadLogList[i];
		fwrite(&log, sizeof(LoadLog), 1, rstFile);
	}
	g_loadLogDataMutex.unlock();


	// ������������
	g_rstDataMutex.lock();
	uint32_t rstDataSize = g_rstData.size();
	fwrite(&rstDataSize, sizeof(rstDataSize), 1, rstFile);
	for (map<uint32_t, ReqLog>::iterator it = g_rstData.begin(); it != g_rstData.end(); ++it)
	{
		ReqLog * log = &it->second;
		fwrite(log, sizeof(ReqLog), 1, rstFile);
	}
	g_rstDataMutex.unlock();

	fclose(rstFile);
	return 0;
}

int ReadySimu()
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
	// �ȴ�·�ɱ��ʼ��
	while (true)
	{
		sleep(1);
		if (!g_serviceTable.empty())
		{
			break;
		}
	}
	return 0;
}
int SimuAll(map<uint32_t, string> & trafficFiles)
{
	if (ReadySimu() < 0)
	{
		return -1;
	}

	// ��ʼ����
	map<int32_t, int32_t> retMap;
	for (map<uint32_t, string>::iterator it = trafficFiles.begin(); it != trafficFiles.end(); it++)
	{
		g_sendLamda = it->first;
		// �������������
		string dataFile = it->second;
		vector<AppReq> traffic;
		if (!ReadTrafficFile(dataFile, traffic))
		{
			cout << "Traffic file read error\n";
			return -2;
		}

		// ��ʼ����
		StartSimu(traffic, g_rstData, retMap);
	}

	//10���Ӻ�ʼ�洢����
	sleep(10);

	string rstFileName = "simu.stress";
	int ret = SaveLogData(rstFileName);
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
	g_serviceTable = serviceTable;// ѹ���ʱ���ܿ�AdminServer
	uint32_t reqCount = 0;

	map<int32_t, int32_t> retMap;
	for (map<int, string>::iterator it = trafficFiles.begin(); it != trafficFiles.end(); it++)
	{
		g_sendLamda = it->first;
		// �������������
		string dataFile = it->second;
		vector<AppReq> traffic;
		if (!ReadTrafficFile(dataFile, traffic))
		{
			cout << "Traffic file read error\n";
			return -2;
		}

		// ��ʼѹ��
		reqCount += traffic.size();
		StartSimu(traffic, g_rstData, retMap);
	}
	//10���Ӻ�ʼ�洢����
	sleep(10);
	string rstFileName = string(compName) + string("_") + string(ep.ip) + string(".stress");
	int ret = SaveLogData(rstFileName);
	return 0;
}
