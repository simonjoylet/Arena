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

	for (uint32_t i = 0; i < n; ++i) 
	{
		AppReq req;
		req.id = i + 1;
		req.interval = d(gen) * MILLION;
		strcpy(req.service, serviceVec[rand() % serviceVec.size()].c_str());
		req.weight = weightVec[rand() % weightVec.size()];
		
		fwrite(&req, sizeof(req), 1, f);
		records.push_back(req);
	}

	fclose(f);
	return 0;
}


int main()
{
	vector<string> services = { "Comp1", "Comp2"};
	vector<uint32_t> weights = { 1, 2, 3 };
	string filePath = "./test.dat";
	GenerateTraffic(1000, 10000, services, weights, filePath);
	
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