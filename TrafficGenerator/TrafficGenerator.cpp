#include <iostream>
#include <random>
#include <stdio.h>
#include "TrafficDefine.h"
using namespace std;
/*
���ģ�����������ǰ���ָ��Ҫ�������������У����洢���ļ��У��ļ���ʽ���£�
 ����id | ʱ����us | �������� | �û�Ȩ��
*/

static const unsigned MILLION = 1000000;

static vector<AppReq> records;

int GenerateTraffic(
	uint32_t lamda, // ����ǿ��
	uint32_t n, // ���ɼ�¼������
	vector<string> serviceVec, // �����������飬���ɼ�¼ʱ�������
	vector<unsigned> weightVec, // Ȩ�����飬���ɼ�¼ʱ�������
	string filePath // �����ļ��洢·��
	)
{
	if (lamda <= 0 || n <= 0 || serviceVec.empty() || weightVec.empty() || filePath.empty())
	{
		return -1;
	}

	FILE * f = fopen(filePath.c_str(), "wb");
	if (f == NULL) return -2;

	std::random_device rd;
	std::default_random_engine gen(rd());
	std::exponential_distribution<> d(lamda);
	srand(d(gen) * MILLION);

	fwrite(&n, sizeof(n), 1, f);
	static uint32_t idCount = 0;
	for (uint32_t i = 0; i < n; ++i) 
	{
		AppReq req;
		req.id = ++idCount;
		req.interval = d(gen) * MILLION;
		strcpy(req.service, serviceVec[rand() % serviceVec.size()].c_str());
		req.weight = weightVec[rand() % weightVec.size()];
		
		fwrite(&req, sizeof(req), 1, f);
		records.push_back(req);
	}

	fclose(f);
	return 0;
}
#include "../SimuClient/ReqAnalytics.h"
int ReadStressFile()
{
	string fileName = "Comp_1_223.3.87.60.stress";
	FILE * stressFile = fopen(fileName.c_str(), "rb");
	if (stressFile == NULL)
	{
		printf("File open failed, path: %s\n", fileName.c_str());
		return -1;
	}

	// ���渺������
	vector<LoadLog> loadLogList;
	uint32_t loadLogCount = 0; 
	fread(&loadLogCount, sizeof(loadLogCount), 1, stressFile);
	for (size_t i = 0; i < loadLogCount; i++)
	{
		LoadLog log;
		fread(&log, sizeof(LoadLog), 1, stressFile);
		loadLogList.push_back(log);
	}


	// ������������
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
}

int main()
{
	// ���Զ�ȡѹ�������ļ�
	ReadStressFile();


	vector<string> services = { "Comp_2"};
	vector<uint32_t> weights = { 1, 2, 3 };
	const char * filePathTemplate = "./Comp_2_%d.dat";
	const int period = 10;
	for (int i = 1; i <= 15; ++i)
	{
		char filePath[32] = {};
		int lamda = 10 * i;
		sprintf(filePath, filePathTemplate, 10 * i);
		GenerateTraffic(lamda, lamda * period, services, weights, filePath);
	}
	
	
	// ���Զ�������ݺ�д��������Ƿ��Ǻϡ�
	// testRead(filePath);
	getchar();
	return 0;
}

void testRead(string filePath)
{
	FILE * f = fopen(filePath.c_str(), "rb");
	uint32_t n = 0;
	fread(&n, sizeof(n), 1, f);
	vector<AppReq> records;
	for (int i = 0; i < n; ++i)
	{
		AppReq req;
		fread(&req, sizeof(req), 1, f);
		records.push_back(req);
	}
	fclose(f);
}