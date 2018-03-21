#ifndef ADMIN_DATA_H
#define ADMIN_DATA_H

#include <map>
#include <set>
#include <string>
#include <mutex>
#include <stdint.h>
#include <vector>
#include "../NodeAgent/node_client.h"
using namespace std;

static const double MAX_UTILIZATION = 0.9; // �����Դ������

namespace localdata
{ 
struct InetAddress
{
	string ip;
	uint16_t port;
	InetAddress() :ip(""), port(0){}
	InetAddress(string _ip, uint16_t _port);
	bool operator<(const InetAddress &);
};

struct NodeInfo
{
	InetAddress addr;
	uint32_t type;			// ʵ������
	uint32_t heatbeat;		// ����������Ĭ��Ϊ0
	float cpuload;
	float diskload;
	map<string, uint32_t> netrtt;
	NodeInfo():type(0), heatbeat(0), cpuload(0), diskload(0){}
};


struct ServiceInfo
{
	uint32_t id;
	string name;
	InetAddress addr;
	uint32_t heatbeat;		// ����������Ĭ��Ϊ0
	uint32_t lamda;
	uint32_t queueLength;
	ServiceInfo() :id(0), heatbeat(0), lamda(0), queueLength(0){}
};

struct RouterItem
{
	string compName;
	string ip;
	uint16_t port;
	double percentage;
	uint16_t pid;
	RouterItem() : compName(""), ip(""), port(0), percentage(0), pid(0){}
};

struct StressInfo
{
	uint32_t lamda;
	double cpuLoad;
	double diskLoad;
	double queueGrowSpeed; // ���������ٶȣ���/s
	StressInfo() : lamda(0), cpuLoad(0), diskLoad(0), queueGrowSpeed(0){}
};
struct CompStress
{
	string name; // �������
	double processTime; // ÿ�������ƽ������ʱ��
	double cpuPerLamda; // ��λ����ǿ���µ�CPU����
	double diskPerLamda; // ��λ����ǿ���µ�disk����
	double cpuBase; // ����ʱ��CPU����
	double diskBase; // ����ʱ��disk����
	vector<StressInfo> stressVec;
	CompStress() :name(""), processTime(0), cpuPerLamda(0),
		diskPerLamda(0), cpuBase(0), diskBase(0){}
};
}

class AdminData
{
public:
	static AdminData * GetInstance();

	// ע��Ľڵ�
	map<string/*ip*/, localdata::NodeInfo> m_nodeList;
	
	// ע��ķ���
	map<uint32_t/*id*/, localdata::ServiceInfo> m_serviceList;
	vector<uint32_t> m_tobeStopedVec;
	
	// �����·�ɱ�
	vector<localdata::RouterItem> m_router;

	// ���������ѹ�����ݱ�
	map<string/*name*/, localdata::CompStress> m_stressMap;

	void InitServiceTable();
	int32_t UpdateServiceTable(); // TODO

	int32_t ReadStressData(string compName, string filePath);

	magna::StartComponentResponse StartComp(localdata::InetAddress & nodeAddr, string compName);

	int32_t GetNewServiceId();
	
	void lock() { m_mutex.lock(); }
	void unlock() { m_mutex.unlock(); }

private:
	static AdminData * m_instance;
	uint32_t m_serviceIdCount;
	std::mutex m_mutex;
	// not allowed
	AdminData();
	~AdminData(){}
};

#endif//ADMIN_DATA_H
