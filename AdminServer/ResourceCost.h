#ifndef RESOURCE_COST_H
#define RESOURCE_COST_H

struct ResourceCost
{
	uint32_t lamda; // ����ǿ�ȣ�qps
	double cpuLoad; // cpu��Դ�����ʣ�ֱ��ȡ�ü��ֵ
	double diskLoad; // ͨ��RDISK���㣬RDISKд������������200MB/s
};

#endif//RESOURCE_COST_H
