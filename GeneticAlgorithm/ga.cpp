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

// 设置关键参数
const string dataFile = "test.data";
uint32_t n = 20;				//组件数量
uint32_t m = 5;				//资源纬度
uint32_t l = 4;				//最大分组规模
// const uint32_t n = 3;				//组件数量
// const uint32_t m = 2;				//资源纬度
// const uint32_t l = 3;				//最大分组规模
const uint32_t popSize = n*l;		//种群大小
const double crossRate = 0.6;		//交叉率
const double mutateRate = 0.3;		//变异率
const double surviveRate = 0.3;		//生存比例
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
	// 生成流量比例
	vector<uint32_t> randVec;
	for (uint32_t i = 0; i < n; ++i)
	{
		randVec.push_back(rand() % 100);
	}
	double sum = accumulate(randVec.begin(), randVec.end(), 0);

	vector<double> flowVec(n);// 每个组件的流量比例
	for (uint32_t i = 0; i < n; ++i)
	{
		flowVec[i] = 100.0 * randVec[i] / sum;
	}

	// 生成测试数据并写入文件
// 	FILE * f = fopen(fileName.c_str(), "wb");
// 	fwrite(&n, sizeof(n), 1, f);
// 	fwrite(&m, sizeof(m), 1, f);
	for (uint32_t i = 0; i < n; ++i)
	{
		vector<double> resVec;
		for (uint32_t j = 0; j < m; ++j)
		{
			double tmp = RandDouble(); // 生成[0,1]之间的随机数
			tmp *= flowVec[i];// 乘上流量比例
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

double GetCod(const vector<double> & resVec) //计算离散系数
{
	double sum = 0;
	for (uint32_t i = 0; i < m; ++i)
	{
		sum += resVec[i];
	}
	// 和为0说明数组为空
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
	double sigma = sqrt(sum / m);// 标准差
	double cod = sigma / average;
	return cod;
}

double GetBalance(const Entity & e)
{
	// 构造结果数据，即每条染色体的资源数组
	vector<vector<double>> rstData(n);
	for (uint32_t i = 0; i < n; ++i)
	{
		const Chrom & tmpChrom = e[i];

		// 一条染色体即为一个组件分组，计算累加的资源数组
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
				index -= 1; // 找到组件编号
			}
			const vector<double> & resVec = data[index];
			for (uint32_t mIndex = 0; mIndex < m; ++mIndex)
			{
				sumResVec[mIndex] += resVec[mIndex];
			}
		}
	}

	// 按照加权平均，计算个体的均衡度
	double sumWeight = 0;// 权重
	double sumCod = 0;// 离散系数
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
	// 计算平衡度并排序
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

	// 先用精英制选择
	vector<Entity> nextPop;
	uint32_t survive = popSize * surviveRate;
	uint32_t elite = survive * eliteRate;
	for (uint32_t i = 0; i < elite; ++i)
	{
		nextPop.push_back(balanceVec[i].second);
	}
	balanceVec.erase(balanceVec.begin(), balanceVec.begin() + elite);

	// 再用轮盘赌选择剩下的
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

	// 返回新种群
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
				index -= 1; // 找到组件编号
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
	// 把a中最好的一条染色体交换给b // todo：可以考虑更加合理的交换方式
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

	// 先移除b中的bestChrom元素
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
	// 随机找到一个个体
	Entity e = pop[rand() % pop.size()];
	for (uint32_t i = 0; i < 1+rand()%l; ++i)
	{
		// 随机交换其中一个非0元素的位置
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

double WorstBalance()
{
	Entity e;
	for (uint32_t i = 0; i < n; ++i)
	{
		Chrom tmp(l);
		tmp[0] = i + 1;
		e.push_back(tmp);
	}
	double worst = GetBalance(e);
	return worst;
}

double GA(Entity & bestEntity)
{
	// 构造初始种群
	vector<Entity> pop;
	InitPop(pop);

	// 迭代
	double bestValue = 0;
	unsigned int genCount = 0;
	while (++genCount)
	{
		// 选择
		bestValue = PopSelect(pop, bestEntity);
		//printf("gen: %d, best: %.4f\n", genCount, bestValue);
		//PrintEntity(bestEntity);
		// 判断迭代终止条件
		if (genCount > n*l*iterMutiple)
		{
			break;
		}

		// 构建下一代种群
		vector<Entity> nextPop = pop;
		while (nextPop.size() < popSize)
		{
			// 交叉
			if (RandDouble() < crossRate)
			{
				nextPop.push_back(PopCross(pop));
			}
			if (nextPop.size() == popSize)
			{
				break;
			}

			// 变异
			if (RandDouble() < mutateRate)
			{
				nextPop.push_back(PopMutate(pop));
			}
		}

		pop = nextPop;
	}

	return bestValue;
}

int main(int argc, char **argv)
{
	InitRand();
	n = atoi(argv[1]);
	m = atoi(argv[2]);
	l = atoi(argv[3]);

	// 创建结果文件
	char rstFileName[32] = {};
	sprintf(rstFileName, "%d_%d_%d.txt", n, m, l);
	fstream rstFile(rstFileName, ios::out);
	char paramStr[32] = {};
	sprintf(paramStr, "n: %d, m: %d, l: %d\n", n, m, l);
	cout << paramStr;
	rstFile << paramStr;

	// 开始仿真试验
	uint32_t simuTimes = 30;
	double sumRatio = 0;
	const uint32_t round = 3; // 同一份数据三次试验为一轮
	for (uint32_t i = 0; i < simuTimes; ++i)
	{
		if (i % round == 0) // 一份数据算三遍。
		{
			GenereteTestData(n, m, dataFile);
		}
		
		
		// 读取实验数据
		//ReadData(dataFile, data);
		// 	vector<double> comp1 = { 0.5847, 0 };
		// 	vector<double> comp2 = { 0.2028, 0.2369 };
		// 	vector<double> comp3 = { 0.051, 0 };
		// 	data.push_back(comp1);
		// 	data.push_back(comp2);
		// 	data.push_back(comp3);

		static double bestValue = 5;
		//bestValue = WorstBalance();
		if (i % round == 0) // 每份数据重置一次最优值
		{
			bestValue = 5;
		}
		Entity bestEntity;
		double tmpValue = GA(bestEntity);
		double optimal = BestBalance();
		double ratio = tmpValue / optimal;

		bestValue = tmpValue < bestValue ? tmpValue : bestValue;
		if (i % round == (round-1)) // 每份数据重置一次最优值
		{
			sumRatio += bestValue / optimal;
		}
		char expInfo[128] = {};
		sprintf(expInfo, "experiment:%d, optimal: %.4f, ratio: %.4f\n", i + 1, optimal, ratio);
		cout << expInfo;
		rstFile << expInfo;
	}

	// 将结果写入文件
	double averageRatio = sumRatio / (simuTimes/round);
	char rstStr[64] = {};
	sprintf(rstStr, "\n\naverage ratio: %.4f\n", averageRatio);
	cout << rstStr;
	rstFile << rstStr;
	rstFile.close();

	getchar();
	return 0;
}

