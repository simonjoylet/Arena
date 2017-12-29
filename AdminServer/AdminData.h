#ifndef ADMIN_DATA_H
#define ADMIN_DATA_H
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
}

class AdminData
{
public:
	static AdminData * GetInstance();

private:
	// ע��Ľڵ�
	map<string/*ip*/, localdata::NodeAddr> m_nodeList;

	// �ڵ��״̬
	map<string/*ip*/, localdata::NodeStatus> m_nodeStatus;

	// ע��ķ���
	map<int32/*id*/, localdata::ServiceAddr> m_serviceList;
	map<string/*name*/, set<int32> > m_serviceDNS;


private:
	static AdminData * m_instance;

	// not allowed
	AdminData();
	~AdminData();
};

#endif//ADMIN_DATA_H
