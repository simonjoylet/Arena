#include "AdminData.h"
#include "../SimuClient/ReqAnalytics.h"

AdminData * AdminData::m_instance = NULL;

AdminData::AdminData()
{
	m_serviceIdCount = 0;
}

AdminData * AdminData::GetInstance()
{
	if (NULL == m_instance)
	{
		m_instance = new AdminData();
	}

	return m_instance;
}

void AdminData::InitServiceTable(vector<localdata::RouterItem> & router)
{
	m_router = router;
// 	for (uint32_t i = 0; i < router.size(); ++i)
// 	{
// 		m_router.push_back(router[i]);
// 	}
}

int32_t AdminData::UpdateServiceTable()
{
	// �������ۼ�ÿ�ַ���ĵ����ʣ���������Ļ���̨���������ж���Դ�Ƿ��á�
	map<string, uint32_t> serviceLamda;
	for (auto it = m_serviceList.begin(); it != m_serviceList.end(); it++)
	{
		localdata::ServiceInfo & serviceInfo = it->second;
		if (serviceLamda.find(serviceInfo.name) == serviceLamda.end())
		{
			serviceLamda[serviceInfo.name] = serviceInfo.lamda;
		}
		else
		{
			serviceLamda[serviceInfo.name] += serviceInfo.lamda;
		}
	}




	// ������ã��ҵ�����Ҫ���·�ɱ��Աȵ�ǰ·�ɱ�������ر������
	// ��������ã����ո��ؾ������·�ɱ���㡣

}


// ��ȡ�����ѹ������
int32_t AdminData::ReadStressData(string compName, string filePath)
{
	FILE * stressFile = fopen(filePath.c_str(), "rb");
	if (stressFile == NULL)
	{
		printf("File open failed, path: %s\n", filePath.c_str());
		return -1;
	}

	// ��ȡ��������
	vector<LoadLog> loadLogList;
	uint32_t loadLogCount = 0;
	fread(&loadLogCount, sizeof(loadLogCount), 1, stressFile);
	for (size_t i = 0; i < loadLogCount; i++)
	{
		LoadLog log;
		fread(&log, sizeof(LoadLog), 1, stressFile);
		loadLogList.push_back(log);
	}


	// ��ȡ��������
	vector<ReqLog> reqLogList;
	uint32_t reqLogCount = 0;
	fread(&reqLogCount, sizeof(reqLogCount), 1, stressFile);
	for (size_t i = 0; i < reqLogCount; ++i)
	{
		ReqLog log;
		fread(&log, sizeof(ReqLog), 1, stressFile);
		reqLogList.push_back(log);
	}
	fclose(stressFile);

	// �������ݲ�����
	vector<localdata::StressInfo> stressVec;

	m_serviceStress[compName] = stressVec;
}

int32_t AdminData::GetNewServiceId()
{
	lock();
	int32_t rst = ++m_serviceIdCount;
	unlock();
	return rst;
}

localdata::InetAddress::InetAddress(string _ip, uint16_t _port)
{
	ip = _ip;
	port = _port;
}

bool localdata::InetAddress::operator<(const InetAddress & param)
{
	if (ip < param.ip)
	{
		return true;
	}
	else if (ip == param.ip)
	{
		return port < param.port;
	}
	else
	{
		return false;
	}
}
