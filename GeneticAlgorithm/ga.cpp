#include <iostream>
#include <random>
#include <time.h>
#include <vector>
#include <stdint.h>
#include <algorithm>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <set>
#include <fstream>
using namespace std;

// ���ùؼ�����
const string dataFile = "test.data";
uint32_t n = 20;				//�������
uint32_t m = 5;				//��Դγ��
uint32_t l = 4;				//�������ģ
// const uint32_t n = 3;				//�������
// const uint32_t m = 2;				//��Դγ��
// const uint32_t l = 3;				//�������ģ
const uint32_t popSize = n*l;		//��Ⱥ��С
const double crossRate = 0.6;		//������
const double mutateRate = 0.3;		//������
const double surviveRate = 0.3;		//�������
const double eliteRate = 0.3;
const uint32_t iterMutiple = 20;
vector<vector<double>> data;

void InitRand()
{
	time_t ts;
	time(&ts);
	srand(ts);
}
double RandDouble()
{
	const uint32_t RAND_RANGE = 10000;
	return 1.0 * (rand() % RAND_RANGE) / RAND_RANGE;
}

int GenereteTestData(uint32_t n, uint32_t m, string fileName)
{
	data.clear();
	// ������������
	vector<uint32_t> randVec;
	for (uint32_t i = 0; i < n; ++i)
	{
		randVec.push_back(rand() % 100);
	}
	double sum = accumulate(randVec.begin(), randVec.end(), 0);

	vector<double> flowVec(n);// ÿ���������������
	for (uint32_t i = 0; i < n; ++i)
	{
		flowVec[i] = 100.0 * randVec[i] / sum;
	}

	// ���ɲ������ݲ�д���ļ�
// 	FILE * f = fopen(fileName.c_str(), "wb");
// 	fwrite(&n, sizeof(n), 1, f);
// 	fwrite(&m, sizeof(m), 1, f);
	for (uint32_t i = 0; i < n; ++i)
	{
		vector<double> resVec;
		for (uint32_t j = 0; j < m; ++j)
		{
			double tmp = RandDouble(); // ����[0,1]֮��������
			tmp *= flowVec[i];// ������������
//			fwrite(&tmp, sizeof(tmp), 1, f);
			resVec.push_back(tmp);
		}
		data.push_back(resVec);
	}
//	fclose(f);

	return 0;
}

int ReadData(string fileName, vector<vector<double>> & data)
{
	data.clear();
	FILE * f = fopen(fileName.c_str(), "rb");
	uint32_t n = 0, m = 0;
	fread(&n, sizeof(n), 1, f);
	fread(&m, sizeof(m), 1, f);
	for (uint32_t i = 0; i < n; ++i)
	{
		vector<double> resVec;
		for (uint32_t j = 0; j < m; ++j)
		{
			double tmp = 0;
			fread(&tmp, sizeof(tmp), 1, f);
			resVec.push_back(tmp);
		}
		data.push_back(resVec);
	}
	fclose(f);
	return 0;
}

typedef vector<uint32_t> Chrom;
typedef vector<vector<uint32_t>> Entity;

void InitEntity(Entity & e)
{
	e.resize(n);
	for (uint32_t i = 0; i < n; ++i)
	{
		e[i].resize(l);
	}
}

void InitPop(vector<Entity> & pop)
{
	for (uint32_t i = 0; i < popSize; ++i)
	{
		Entity tmpEntity;
		InitEntity(tmpEntity);
		for (uint32_t nIndex = 0; nIndex < n;)
		{
			uint32_t chromIndex = rand() % n;
			uint32_t geneIndex = rand() % l;
			if (tmpEntity[chromIndex][geneIndex] == 0)
			{
				tmpEntity[chromIndex][geneIndex] = nIndex + 1;
				++nIndex;
			}
		}
		pop.push_back(tmpEntity);
	}
}

bool testEntity(const Entity & tmpEntity)
{
	vector<uint32_t> test;
	for (uint32_t chromIndex = 0; chromIndex < n; ++chromIndex)
	{
		for (uint32_t geneIndex = 0; geneIndex < l; ++geneIndex)
		{
			test.push_back(tmpEntity[chromIndex][geneIndex]);
		}
	}
	sort(test.begin(), test.end());
	reverse(test.begin(), test.end());
	bool rst = accumulate(test.begin(), test.end(), 0) == (1 + n)*n / 2;
	return rst;
}

double GetCod(const vector<double> & resVec) //������ɢϵ��
{
	double sum = 0;
	for (uint32_t i = 0; i < m; ++i)
	{
		sum += resVec[i];
	}
	// ��Ϊ0˵������Ϊ��
	if (sum == 0)
	{
		return 0;
	}

	double average = sum / m;
	sum = 0;
	for (uint32_t i = 0; i < m; ++i)
	{
		sum += (resVec[i] - average) * (resVec[i] - average);
	}
	double sigma = sqrt(sum / m);// ��׼��
	double cod = sigma / average;
	return cod;
}

double GetBalance(const Entity & e)
{
	// ���������ݣ���ÿ��Ⱦɫ�����Դ����
	vector<vector<double>> rstData(n);
	for (uint32_t i = 0; i < n; ++i)
	{
		const Chrom & tmpChrom = e[i];

		// һ��Ⱦɫ�弴Ϊһ��������飬�����ۼӵ���Դ����
		vector<double> & sumResVec = rstData[i];
		sumResVec.resize(m);
		for (uint32_t j = 0; j < l; ++j)
		{
			uint32_t index = tmpChrom[j];
			if (index == 0)
			{
				continue;
			}
			else
			{
				index -= 1; // �ҵ�������
			}
			const vector<double> & resVec = data[index];
			for (uint32_t mIndex = 0; mIndex < m; ++mIndex)
			{
				sumResVec[mIndex] += resVec[mIndex];
			}
		}
	}

	// ���ռ�Ȩƽ�����������ľ����
	double sumWeight = 0;// Ȩ��
	double sumCod = 0;// ��ɢϵ��
	for (uint32_t i = 0; i < n; ++i)
	{
		vector<double> & resVec = rstData[i];
		double tmpWeight = *max_element(resVec.begin(), resVec.end());
		double tmpCod = GetCod(resVec);
		sumWeight += tmpWeight;
		sumCod += tmpWeight*tmpCod;
	}
	double balance = sumCod / sumWeight;
	return balance;
}

double PopSelect(vector<Entity> & pop, Entity & bestEntity)
{
	// ����ƽ��Ȳ�����
	vector<pair<double, Entity>> balanceVec;
	for (uint32_t i = 0; i < popSize; ++i)
	{
		Entity & e = pop[i];
		double balance = GetBalance(e);
		balanceVec.push_back(make_pair(balance, e));
	}
	sort(balanceVec.begin(), balanceVec.end());
	bestEntity = balanceVec.begin()->second;
	double bestBalance = balanceVec.begin()->first;

	// ���þ�Ӣ��ѡ��
	vector<Entity> nextPop;
	uint32_t survive = popSize * surviveRate;
	uint32_t elite = survive * eliteRate;
	for (uint32_t i = 0; i < elite; ++i)
	{
		nextPop.push_back(balanceVec[i].second);
	}
	balanceVec.erase(balanceVec.begin(), balanceVec.begin() + elite);

	// �������̶�ѡ��ʣ�µ�
	uint32_t selectSurvive = popSize * surviveRate * (1-eliteRate);
	while (nextPop.size() < survive)
	{
		static uint32_t i = 0;
		i = (++i) % balanceVec.size();
		if (RandDouble() < (1.0 / (i+2)))
		{
			nextPop.push_back(balanceVec[i].second);
		}
	}

	// ��������Ⱥ
	pop = nextPop;
	return bestBalance;
}

uint32_t GetBestChrom(Entity & e)
{
	uint32_t bestIndex = 0;
	double minBalance = 100000;
	for (uint32_t i = 0; i < n; ++i)
	{
		Chrom & tmpChrom = e[i];
		if (accumulate(tmpChrom.begin(), tmpChrom.end(), 0) == 0)
		{
			continue;
		}
		vector<double> sumResVec(m);
		for (uint32_t j = 0; j < l; ++j)
		{
			uint32_t index = tmpChrom[j];
			if (index == 0)
			{
				continue;
			}
			else
			{
				index -= 1; // �ҵ�������
			}
			const vector<double> & resVec = data[index];
			for (uint32_t mIndex = 0; mIndex < m; ++mIndex)
			{
				sumResVec[mIndex] += resVec[mIndex];
			}
		}
		double tmpBalance = GetCod(sumResVec);
		if (tmpBalance < minBalance)
		{
			minBalance = tmpBalance;
			bestIndex = i;
		}
	}
	return bestIndex;
}

int32_t GetEmptyChrom(Entity & e)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		Chrom & tmpChrom = e[i];
		if (accumulate(tmpChrom.begin(), tmpChrom.end(), 0) == 0)
		{
			return i;
		}
		
	}
	return -1;
}

Entity PopCross(vector<Entity> & pop)
{
	// ��a����õ�һ��Ⱦɫ�彻����b // todo�����Կ��Ǹ��Ӻ���Ľ�����ʽ
	Entity & a = pop[rand() % pop.size()];
	uint32_t bestIndex = GetBestChrom(a);
	

	Entity b;
	while (1)
	{
		Entity & tmpB = pop[rand() % pop.size()];
		
		if (GetEmptyChrom(tmpB) < 0)
		{
			continue;
		}
		b = tmpB;
		break;
	}
	int32_t emptyIndex = GetEmptyChrom(b);

	// ���Ƴ�b�е�bestChromԪ��
	Chrom & bestChrom = a[bestIndex];
	vector<uint32_t> rmVec;
	for (uint32_t i = 0; i < l; ++i)
	{
		if (bestChrom[i] != 0)
		{
			rmVec.push_back(bestChrom[i]);
		}
	}
	for (uint32_t rmIndex = 0; rmIndex < rmVec.size(); ++rmIndex)
	{
		uint32_t rmTarget = rmVec[rmIndex];
		for (uint32_t i = 0; i < n; ++i)
		{
			Chrom & tmpChrom = b[i];
			for (uint32_t j = 0; j < l; ++j)
			{
				if (tmpChrom[j] == rmTarget)
				{
					tmpChrom[j] = 0;
				}
			}
		}
	}
	
	b[emptyIndex] = bestChrom;
	return b;
}

Entity PopMutate(vector<Entity> & pop)
{
	// ����ҵ�һ������
	Entity e = pop[rand() % pop.size()];
	for (uint32_t i = 0; i < 1+rand()%l; ++i)
	{
		// �����������һ����0Ԫ�ص�λ��
		uint32_t chromIndex = 0;
		uint32_t geneIndex = 0;
		uint32_t element = 0;
		while (1)
		{
			chromIndex = rand() % n;
			geneIndex = rand() % l;
			element = e[chromIndex][geneIndex];
			if (element != 0)
			{
				break;
			}
		}

		uint32_t randChromIndex = rand() % n;
		uint32_t randGeneIndex = rand() % l;

		e[chromIndex][geneIndex] = e[randChromIndex][randGeneIndex];
		e[randChromIndex][randGeneIndex] = element;
	}
	
	return e;
}

void PrintEntity(Entity & e)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		Chrom & tmpChrom = e[i];
		if (accumulate(tmpChrom.begin(),tmpChrom.end(),0) == 0)
		{
			continue;
		}
		printf("\t");
		for (uint32_t j = 0; j < l; ++j)
		{
			if (tmpChrom[j]==0)
			{
				continue;
			}
			cout << tmpChrom[j] << ",";
		}
		printf("\n");
	}
}

double BestBalance()
{
	vector<double> sumRes(m);
	for (uint32_t j = 0; j < m; ++j)
	{
		for (uint32_t i = 0; i < n; ++i)
		{
			sumRes[j] += data[i][j];
		}
	}
	double optimal = GetCod(sumRes);
	return optimal;
}

int main(int argc, char **argv)
{
	InitRand();
	n = atoi(argv[1]);
	m = atoi(argv[2]);
	l = atoi(argv[3]);
	printf("m: %d, n: %d, l: %d\n", n, m, l);
	uint32_t simuTimes = 100;
	double sumRatio = 0;
	for (uint32_t i = 0; i < simuTimes; ++i)
	{
		GenereteTestData(n, m, dataFile);

		// ��ȡʵ������
		//ReadData(dataFile, data);
		// 	vector<double> comp1 = { 0.5847, 0 };
		// 	vector<double> comp2 = { 0.2028, 0.2369 };
		// 	vector<double> comp3 = { 0.051, 0 };
		// 	data.push_back(comp1);
		// 	data.push_back(comp2);
		// 	data.push_back(comp3);
		BestBalance();
		// �����ʼ��Ⱥ
		vector<Entity> pop;
		InitPop(pop);

		// ����
		Entity bestEntity;
		double bestValue = 0;
		unsigned int genCount = 0;
		while (++genCount)
		{
			// ѡ��
			bestValue = PopSelect(pop, bestEntity);
			//printf("gen: %d, best: %.4f\n", genCount, bestValue);
			//PrintEntity(bestEntity);
			// �жϵ�����ֹ����
			if (genCount > n*l*iterMutiple)
			{
				break;
			}

			// ������һ����Ⱥ
			vector<Entity> nextPop = pop;
			while (nextPop.size() < popSize)
			{
				// ����
				if (RandDouble() < crossRate)
				{
					nextPop.push_back(PopCross(pop));
				}
				if (nextPop.size() == popSize)
				{
					break;
				}

				// ����
				if (RandDouble() < mutateRate)
				{
					nextPop.push_back(PopMutate(pop));
				}
			}

			pop = nextPop;
		}

		double optimal = BestBalance();
		double ratio = bestValue / optimal;
		sumRatio += ratio;
		printf("experiment:%d, optimal: %.4f, ratio: %.4f\n",i+1, optimal, ratio);
	}
	double averageRatio = sumRatio / simuTimes;
	printf("\n\naverage ratio: %.4f\n", averageRatio);

	// �����д���ļ�
	char rstFileName[32] = {};
	sprintf(rstFileName, "%d_%d_%d.txt", n, m, l);
	fstream rstFile(rstFileName, ios::out);
	rstFile << averageRatio;
	rstFile.close();

	getchar();
	return 0;
}

