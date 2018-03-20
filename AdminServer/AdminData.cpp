#include "AdminData.h"
#include "../SimuClient/ReqAnalytics.h"
#include <math.h>

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

void AdminData::InitServiceTable(vector<localdata::RouterItem> & router)
{
	m_router = router;
// 	for (uint32_t i = 0; i < router.size(); ++i)
// 	{
// 		m_router.push_back(router[i]);
// 	}
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
	map<uint32_t, vector<LoadLog>> loadMap;
	uint32_t loadLogCount = 0;
	fread(&loadLogCount, sizeof(loadLogCount), 1, stressFile);
	for (size_t i = 0; i < loadLogCount; i++)
	{
		LoadLog log;
		fread(&log, sizeof(LoadLog), 1, stressFile);
		loadMap[log.sendLamda].push_back(log);
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
	// ����������͵���ǿ����ƽ������Դ�����ʺʹ���ʱ��
	localdata::CompStress compStress;
	compStress.name = compName;
	vector<localdata::StressInfo> & stressVec = compStress.stressVec;
	uint32_t finishCount = 0;
	for (auto it = loadMap.begin(); it != loadMap.end(); ++it)
	{
		localdata::StressInfo stressInfo;
		stressInfo.lamda = it->first;
		vector<LoadLog> & loadList = it->second;
		uint32_t logCount = 0;
		uint32_t firstId = 0, lastId = 0; // ���ڼ�����������ٶ�
		for (uint32_t i = 0; i < loadList.size(); ++i)
		{
			++logCount;
			stressInfo.cpuLoad += loadList[i].cpuLoad;
			stressInfo.diskLoad += loadList[i].diskLoad;

			uint32_t curId = loadList[i].sendId;
			ReqLog & currentReq = reqLogList[curId];
			if (currentReq.localEnd > 0)
			{
				++finishCount;
				compStress.processTime += currentReq.processTime;
				lastId = curId;
			}
			
			if (curId == reqLogList.size()) // ���һ����¼Ҳֻͳ��һ�Ρ�
			{
				break;
			}
		}
		// ������������ٶ�
		if (reqLogList[loadList[0].sendId].localEnd > 0)
		{
			firstId = loadList[0].sendId;
		}
		ReqLog & firstLog = reqLogList[firstId];
		ReqLog & lastLog = reqLogList[lastId];
		if ((lastLog.localBegin - firstLog.localBegin) != 0)
		{
			stressInfo.queueGrowSpeed = 1000.0 * (lastLog.queueLength - firstLog.queueLength) / (lastLog.localBegin - firstLog.localBegin);
		}
		
		stressInfo.cpuLoad /= logCount;
		stressInfo.diskLoad /= logCount;
		

		stressVec.push_back(stressInfo);
	}

	// ����ÿ�������ƽ������ʱ��
	if (finishCount > 0)
	{
		compStress.processTime /= finishCount;
	}

	// ���㵥λ����ǿ�ȵ���Դ����
	for (uint32_t i = 0; i < stressVec.size(); ++i)
	{
		if (stressVec[i].queueGrowSpeed > 1)
		{
			double deltaLamda = (stressVec[i - 1].lamda - stressVec[0].lamda);
			compStress.cpuPerLamda = ((stressVec[i - 1].cpuLoad - compStress.cpuBase) - (stressVec[0].cpuLoad - compStress.cpuBase)) / deltaLamda;
			compStress.diskPerLamda = ((stressVec[i - 1].diskLoad - compStress.diskBase) - (stressVec[0].diskLoad - compStress.diskBase)) / deltaLamda;
			compStress.cpuPerLamda = compStress.cpuPerLamda > 0 ? compStress.cpuPerLamda : 0; // ��Դ���ı������0
			compStress.diskPerLamda = compStress.diskPerLamda > 0 ? compStress.diskPerLamda : 0; 
			break;
		}
		
	}

	m_stressMap[compName] = compStress;
	return 0;
}

int32_t AdminData::UpdateServiceTable()
{
	// �������ۼ�ÿ�ַ���ĵ�����
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

	// ������������Ļ���̨����
	double cpuNeed = 0, diskNeed = 0;
	for (auto it = serviceLamda.begin(); it != serviceLamda.end(); it++)
	{
		string serviceName = it->first;
		localdata::CompStress & compStress = m_stressMap[serviceName];
		uint32_t curLamda = it->second;
		cpuNeed += curLamda * compStress.cpuPerLamda;
		diskNeed += curLamda * compStress.diskPerLamda;
	}
	uint32_t needMachineAmount = ceil((cpuNeed > diskNeed ? cpuNeed : diskNeed) / MAX_UTILIZATION);

	// ������ã������ڵ�ǰ·�ɱ�Ļ��������޸ģ�׷���ؾ��⡣
	if (needMachineAmount <= m_nodeList.size())
	{
		//������Ҫ�����ĸ�������ĸ���
		map<string, uint32_t> serviceAmountMap;
		for (auto it = serviceLamda.begin(); it != serviceLamda.end(); it++)
		{

		}

		// ������������������������ռ��

	}
	// ��������ã�����������ֵ����·�ɱ���㡣
	else
	{

	}
	
	return -1;
}

