#include "SimuFunc.h"

int main(int argc, char **argv)
{
	AdminClient::Init("../AdminServer/admin_client.conf");
	CompClient::Init("../Comp/comp_client.conf");

	g_adminProxy = new AdminClient;

	// �����Ƿ���Է���AdminServer
	if (!TestAccessAdminServer())
	{
		cout << "AdminServer not available\n";
		return -1;
	}

	// �������������
	string dataFile = "../TrafficGenerator/test.dat";
	vector<AppReq> traffic;
	if (!ReadTrafficFile(dataFile, traffic))
	{
		cout << "Traffic file read error\n";
		return -2;
	}

	// ��ȡ����·�ɱ�
	map<string, ServiceSelector> serviceTable;
	if (UpdateServiceTable(serviceTable) < 0)
	{
		cout << "Fail to get ServiceTable\n";
		return -3;
	}
	g_serviceTable = &serviceTable;

	// ��AdminServer�����������󣬲���¼��ʼ�ͽ�ֹ��ʱ���
	vector<ReqLog> rstData;
	map<int32_t, int32_t> retMap;
	StartSimu(traffic, rstData, retMap);

	
	return 0;
}


