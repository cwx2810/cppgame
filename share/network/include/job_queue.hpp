#ifndef _JOB_QUEUE_H
#define _JOB_QUEUE_H

#include <queue>
#include <boost/noncopyable.hpp>
#include <boost/utility/value_init.hpp>
#include <boost/concept_check.hpp>
#include <boost/thread.hpp>

//使用条件变量作为并发管理的手段
//一个可用于多线程环境下的阻塞队列，可以用于生产者-消费者模式
template<typename Job>
class  job_queue:boost::noncopyable
{
public:
	typedef Job job_type;                                  //元素类型定义
	typedef std::deque<job_type> queue_type;               //容器类型定义
	typedef boost::mutex    mutex_type;                    //互斥量类型定义
	typedef typename mutex_type::scoped_lock lock_type;    //锁定义
	typedef boost::condition_variable_any condition_type;  //条件变量定义

	//检查某种概念是否符合某些操作
	//此处要求有拷贝构造函数
	BOOST_CONCEPT_ASSERT((boost::SGIAssignable<job_type>));//概念检查，检查是否有赋值操作

	//检查默认构造函数
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<job_type>));
//构造函数
public:
	job_queue();
public:
	//操作很简单，主要就是出队和入队
	void push(const job_type &x);
	job_type pop();
	void stop();
//成员变量
private:
	queue_type m_queue;        //内部容器
	mutex_type m_mutex;        //互斥量定义
	condition_type m_hasJob;   //条件变量定义，意思是看是不是还有这个数据
	bool   m_stop_flag;        //一个用于停止工作的标志
};


template<typename Job>
job_queue<Job>::job_queue():m_stop_flag(false)//初始化为停止状态
{
}

template<typename Job>
void job_queue<Job>::push(const job_type &x)    //任务入队   
{
	lock_type lock(m_mutex);    //锁定互斥量
	m_queue.push_back(x);       //入队
	m_hasJob.notify_one();      //通知一个等待的线程可以取数据
}


template<typename Job>
typename job_queue<Job>::job_type job_queue<Job>::pop() //任务出队 
{
	lock_type lock(m_mutex);  //锁定互斥量
	while (m_queue.empty() && !m_stop_flag)  //当队列空并且没有要求停止时
	{
		m_hasJob.wait(m_mutex);              //条件变量等待
	}

	if (m_stop_flag)                         //强制停止队列
	{
		return boost::initialized_value;     //相当于return job_type()
	}

	if (m_queue.empty())                     //队空
	{
		return boost::initialized_value;
	}

	job_type tmp = m_queue.front();          //取队列前端的元素
	m_queue.pop_front();
	return tmp;
}

template<typename Job>
void job_queue<Job>::stop()       //停止队列的工作         
{
	m_stop_flag = true;
	m_hasJob.notify_all();        //通知所有等待的线程
}

#endif