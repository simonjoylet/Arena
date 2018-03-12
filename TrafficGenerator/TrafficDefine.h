#ifndef TRAFFIC_DEFINE_H
#define TRAFFIC_DEFINE_H

#include <string>
#include <stdint.h>

struct AppReq
{
	uint32_t id; // ����id
	uint32_t interval; // ����ʱ��������λus
	char service[32]; // ��������
	uint32_t weight; // ����Ȩ��
	AppReq()
	{
		id = 0;
		interval = 0;
		memset(service, 0, sizeof(service));
		weight = 0;
	}
};

#endif//TRAFFIC_DEFINE_H
