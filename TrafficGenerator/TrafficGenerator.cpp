#include <iostream>
#include <random>
#include <iomanip>
#include <string>
#include <map>
using namespace std;
/*
���ģ�����������ǰ���ָ��Ҫ�������������У����洢���ļ��У��ļ���ʽ���£�
 ����id | ʱ����us | �������� | �û�Ȩ��
*/



int main()
{
	std::random_device rd;
	std::default_random_engine gen(rd());

	// ����վ����ƽ��ÿ��1000�Σ�
	// ���´η���ǰҪ���ٺ����ʱ�䣿
	std::exponential_distribution<> d(1000);

	std::map<int, int> hist;
	for (int n = 0; n < 100; ++n) {
		cout << d(gen) * 1000 << endl;
	}

	return 0;
}