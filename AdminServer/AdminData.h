#ifndef ADMIN_DATA_H
#define ADMIN_DATA_H
#include <google/protobuf/stubs/common.h>
using google::protobuf::int32;

#include <map>
#include <set>
#include <string>
#include <mutex>

using std::set;
using std::map;
using std::string;

namespace localdata
{ 
struct InetAddress
{
	string ip;
	int32 port;
	InetAddress() :port(0){}
	InetAddress(string _ip, int32 _port);
	bool operator<(const InetAddress &);
};

struct NodeInfo
{
	InetAddress addr;
	int32 type;			// ʵ������
	int32 heatbeat;		// ����������Ĭ��Ϊ0
	float cpuload;
	float diskload;
	map<string, int32> netrtt;
	NodeInfo():type(0), heatbeat(0), cpuload(0), diskload(0){}
};

struct InterfaceData
{
	double lamda;
	double averageTime;
	InterfaceData() :lamda(0), averageTime(0){}
};

struct ServiceInfo
{
	int32 id;
	string name;
	InetAddress addr;
	int32 heatbeat;		// ����������Ĭ��Ϊ0
	map<string, InterfaceData> interfaceStatus;
	ServiceInfo() :id(0), heatbeat(0){}
};


}

class AdminData
{
public:
	static AdminData * GetInstance();

	// ע��Ľڵ�
	map<string/*ip*/, localdata::NodeInfo> m_nodeList;
	
	// ע��ķ���
	map<int32/*id*/, localdata::ServiceInfo> m_serviceList;
	
	int32_t GetServiceTable() { return 0; }

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
