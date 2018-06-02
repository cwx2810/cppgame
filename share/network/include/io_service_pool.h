//io_service
//1. 在同步模式下， 程序发起一个IO操作，向io_service提交请求，io_service将操作转交给系统
//   同步的等待。当IO操作完成后，操作系统通知io_service,然后io_service将结果发回给程序
//2. 在异步模式下，需定义用于回调的完成处理函数
//3. io_service::run()-->启动前摄器事件处理循环，阻塞等待所有的操作完成并分派事件

#ifndef _IO_SERVICE_POOL_H
#define _IO_SERVICE_POOL_H

#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/functional/factory.hpp>       //工厂函数对象
#include <boost/ptr_container/ptr_vector.hpp> //指针容器

#include "comm.h"

//持多个线程的io_service_pool,其中的每一个线程里都运行着一个并发事件处理器（io_service）,
//可充分多核CPU的能力
class io_service_pool:boost::noncopyable
{
public:

	//定义几个类型
	typedef boost::asio::io_service ios_type;
	typedef boost::asio::io_service::work work_type;
	typedef boost::ptr_vector<ios_type>  io_services_type;   //io_service是不可拷贝的，使用指针容器来管理
	typedef boost::ptr_vector<work_type> works_type;         //多个work集合
//构造函数
public:
	explicit io_service_pool(int n = 4);//创建4个线程，一般cpu是双核，就乘以2
public:
	ios_type& get();  //使用轮询算法来调度多个线程的io_service对象
	void start();     //使用线程组来启动io_service
	void run();       //启动线程组以后进行阻塞等待的操作
	void stop();      //停止运行，清空数据
private:
	void init(int n);      //创建多个io_service对象并加入到指针容器
//成员变量
private:
	io_services_type m_io_services;    //指针容器存储
	works_type       m_works;          //用work保证io_service运转
	boost::thread_group m_threads;     //线程池
	std::size_t   m_next_io_service;   //用于轮询得到io_service
};



#endif
