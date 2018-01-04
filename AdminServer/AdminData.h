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
	InetAddress(string ip, int32 port);
	bool operator<(const InetAddress &);
};

struct NodeAddr
{
	InetAddress addr;
	int mips;			// cpu��������������ָ��ÿ��
	int32 heatbeat;		// ����������Ĭ��Ϊ0
};


struct NodeStatus
{
	float cpuload;
	map<string, int32> netrtt;
};

struct ServiceAddr
{
	int32 id;
	string name;
	InetAddress addr;
	int32 heatbeat;		// ����������Ĭ��Ϊ0
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

	// ע��Ľڵ�
	map<string/*ip*/, localdata::NodeAddr> m_nodeList;

	// �ڵ��״̬
	map<string/*ip*/, localdata::NodeStatus> m_nodeStatus;

	// ע��ķ���
	map<int32/*id*/, localdata::ServiceAddr> m_serviceList;

	// �����״̬
	map<int32/*id*/, localdata::ServiceStatus> m_serviceStatus;

	// �����DNS
	map<string/*name*/, set<int32>/*����id����*/> m_serviceDNS;


private:
	static AdminData * m_instance;

	// not allowed
	AdminData(){}
	~AdminData(){}
};

#endif//ADMIN_DATA_H
