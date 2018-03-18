#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class Semaphore
{
public:
	Semaphore(int value = 1) :m_count(value) {}

	void wait()
	{
		unique_lock<mutex> lck(m_mutex);
		if (--m_count < 0)//��Դ��������߳�
			m_cv.wait(lck);
	}

	void signal()
	{
		unique_lock<mutex> lck(m_mutex);
		if (++m_count <= 0)//���̹߳��𣬻���һ��
			m_cv.notify_one();
	}

private:
	int m_count;
	mutex m_mutex;
	condition_variable m_cv;
};


#endif//SEMAPHORE_H