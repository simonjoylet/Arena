#ifndef ADMIN_DATA_H
#define ADMIN_DATA_H
#include <google/protobuf/stubs/common.h>
using google::protobuf::int32;

#include <map>
#include <set>
#include <string>

using std::set;
using std::map;
using std::string;

namespace localdata
{ 
struct InetAddress
{
	string ip;
	int32 port;
	InetAddress();
	InetAddress(string _ip, int32 _port);
	bool operator<(const InetAddress &);
};

struct NodeInfo
{
	InetAddress addr;
	int32 mips;			// cpu处理能力，百万指令每秒
	int32 heatbeat;		// 心跳计数，默认为0
	NodeInfo()
		:mips(0), heatbeat(0){}
};


struct NodeStatus
{
	float cpuload;
	map<string, int32> netrtt;
	NodeStatus() : cpuload(0){}
};

struct ServiceInfo
{
	int32 id;
	string name;
	InetAddress addr;
	int32 heatbeat;		// 心跳计数，默认为0
};

struct ServiceStatus
{
	map<string, int32> interfaceTime;

};

}

class AdminData
{
public:
	static AdminData * GetInstance();

	// 注册的节点
	map<string/*ip*/, localdata::NodeInfo> m_nodeList;

	// 节点的状态
	map<string/*ip*/, localdata::NodeStatus> m_nodeStatus;

	// 注册的服务
	map<int32/*id*/, localdata::ServiceInfo> m_serviceList;

	// 服务的状态
	map<int32/*id*/, localdata::ServiceStatus> m_serviceStatus;

	int32_t GetServiceTable() { return 0; }


private:
	static AdminData * m_instance;

	// not allowed
	AdminData(){}
	~AdminData(){}
};

#endif//ADMIN_DATA_H
