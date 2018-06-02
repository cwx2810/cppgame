#include "io_service_pool.h"

//构造函数
io_service_pool::io_service_pool(int n):m_next_io_service(0)
{
	//保证至少有一个线程被创建
	if (n <= 0)
	{
		n = 1;
	}
	//调用下面的创建函数
	init(n);
}

//创建多个ioservice，用work来监视
void io_service_pool::init(int n)
{
	for (int i = 0; i < n; ++i)
	{
		//用工厂函数对象来创建
		m_io_services.push_back(boost::factory<ios_type*>()());
		m_works.push_back(boost::factory<work_type*>()(m_io_services.back()));
	}
}


//轮询获取io_service
io_service_pool::ios_type &io_service_pool::get()
{
	//service大于等于4的时候就让他等于0，从头开始，从而实现了轮询
	if (m_next_io_service >= m_io_services.size())
	{
		m_next_io_service = 0;
	}
	return m_io_services[m_next_io_service++];
}

//启动事件循环，非阻塞
void io_service_pool::start()
{
	//大于0说明已经有线程了，说明已经开始了
	if (m_threads.size() > 0)
	{
		return;
	}
	//如果没有就用迭代器遍历对象，启动事件循环
	BOOST_FOREACH(ios_type &ios, m_io_services)
	{
		//使用线程执行io_service::run()启动事件处理循环

		//ref显式告诉编译器 这里用引用传递；否则的话，就是值传递。
		m_threads.create_thread(boost::bind(&ios_type::run, boost::ref(ios)));
	}
}


//阻塞
void io_service_pool::run()
{
	start();
	m_threads.join_all();  //阻塞等待
}

void io_service_pool::stop()
{
	//首先清除所有io_service的工作
	m_works.clear();

	//依次调用io_service的stop来停止运行
	std::for_each(m_io_services.begin(), 
		          m_io_services.end(), 
				  boost::bind(&ios_type::stop, _1));
}