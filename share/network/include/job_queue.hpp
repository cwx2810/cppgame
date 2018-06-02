#ifndef _JOB_QUEUE_H
#define _JOB_QUEUE_H

#include <queue>
#include <boost/noncopyable.hpp>
#include <boost/utility/value_init.hpp>
#include <boost/concept_check.hpp>
#include <boost/thread.hpp>

//ʹ������������Ϊ����������ֶ�
//һ�������ڶ��̻߳����µ��������У���������������-������ģʽ
template<typename Job>
class  job_queue:boost::noncopyable
{
public:
	typedef Job job_type;                                  //Ԫ�����Ͷ���
	typedef std::deque<job_type> queue_type;               //�������Ͷ���
	typedef boost::mutex    mutex_type;                    //���������Ͷ���
	typedef typename mutex_type::scoped_lock lock_type;    //������
	typedef boost::condition_variable_any condition_type;  //������������

	//���ĳ�ָ����Ƿ����ĳЩ����
	//�˴�Ҫ���п������캯��
	BOOST_CONCEPT_ASSERT((boost::SGIAssignable<job_type>));//�����飬����Ƿ��и�ֵ����

	//���Ĭ�Ϲ��캯��
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<job_type>));
//���캯��
public:
	job_queue();
public:
	//�����ܼ򵥣���Ҫ���ǳ��Ӻ����
	void push(const job_type &x);
	job_type pop();
	void stop();
//��Ա����
private:
	queue_type m_queue;        //�ڲ�����
	mutex_type m_mutex;        //����������
	condition_type m_hasJob;   //�����������壬��˼�ǿ��ǲ��ǻ����������
	bool   m_stop_flag;        //һ������ֹͣ�����ı�־
};


template<typename Job>
job_queue<Job>::job_queue():m_stop_flag(false)//��ʼ��Ϊֹͣ״̬
{
}

template<typename Job>
void job_queue<Job>::push(const job_type &x)    //�������   
{
	lock_type lock(m_mutex);    //����������
	m_queue.push_back(x);       //���
	m_hasJob.notify_one();      //֪ͨһ���ȴ����߳̿���ȡ����
}


template<typename Job>
typename job_queue<Job>::job_type job_queue<Job>::pop() //������� 
{
	lock_type lock(m_mutex);  //����������
	while (m_queue.empty() && !m_stop_flag)  //�����пղ���û��Ҫ��ֹͣʱ
	{
		m_hasJob.wait(m_mutex);              //���������ȴ�
	}

	if (m_stop_flag)                         //ǿ��ֹͣ����
	{
		return boost::initialized_value;     //�൱��return job_type()
	}

	if (m_queue.empty())                     //�ӿ�
	{
		return boost::initialized_value;
	}

	job_type tmp = m_queue.front();          //ȡ����ǰ�˵�Ԫ��
	m_queue.pop_front();
	return tmp;
}

template<typename Job>
void job_queue<Job>::stop()       //ֹͣ���еĹ���         
{
	m_stop_flag = true;
	m_hasJob.notify_all();        //֪ͨ���еȴ����߳�
}

#endif