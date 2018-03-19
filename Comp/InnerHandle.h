#ifndef INNER_HANDLE_H
#define INNER_HANDLE_H
#include "comp.pb.h"
#include <stdint.h>
#include "phxrpc/network.h"
#include "../SimuClient/simu_client.h"
#include <list>
extern SimuClient * g_simuProxy;
extern std::string g_compName;

struct ReqWaitInfo
{
	uint32_t id;
	uint32_t compLamda; // ���󵽴�ʱ�ĵ���ǿ��
	uint32_t queueLength; // ���󵽴�ʱ�Ķ��г���
	uint32_t queueBegin; // �Ŷ�ʱ�䣬ms
	uint32_t queueEnd; // �Ŷ�ʱ�䣬ms
	ReqWaitInfo() :id(0), compLamda(0), queueLength(0), queueBegin(0), queueEnd(0){}
};

extern std::list<uint64_t> g_arriveListForLamda;
uint32_t GetLamda();

int32_t InnerHandle(const magna::AppRequest &req, magna::AppResponse *resp);

#endif//INNER_HANDLE_H
