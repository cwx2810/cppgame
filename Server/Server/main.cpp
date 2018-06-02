#include <iostream>  
using namespace std;

#include "comm.h"  
#include "test_tcp_message.h" 
#include "io_service_pool.h"

#include "job_queue.hpp"
#include "worker.h"

typedef job_queue<std::string> queue_type;

queue_type q;


bool print_string(const std::string& str)
{
	cout << "test worker:" << str << endl;
	return true;
}

//处理接收到的数据
void proc_data(socket_type sock)
{
	//输出客户端的地址
	cout << "client connected :" << sock->remote_endpoint().address() << endl;

	//不停的处理
	while (true)
	{
		//捕获可能发生的网络异常，比如客户端突然断开  
		try
		{
			//使用字符数组，因为网络底层数据都是字符流，假设有100个，初始化为0
			//vector<char> str(100, 0);

			//读取客户端发送的数据  
			//同步机制，阻塞读取数据  
			//sock->read_some(buffer(str));
			//cout << "recv from:" << sock->remote_endpoint().address() << endl;
			//通过取首地址的方法取出获取的数据
			//cout << "data:" << &str[0] << endl;
			//cout << endl;

			//将客户端发送的数据回显给客户端  
			//sock->write_some(buffer(str));


			cout << "///////////////////////////////////////////" << endl;
			cout << "recv data from client:" << endl;
			std::string strData = read_data(sock);

			q.push(strData);

			cout << endl;
			//cout << "send data to client:" << endl;
			//send_data(sock, strData);
			cout << "///////////////////////////////////////////" << endl << endl;
		}

		catch (std::exception& e)
		{
			cout << e.what() << endl;
			//跳出循环，断开连接
			break;
		}
	}
}


//线程接收函数 
void proc_accept(io_service_pool &pool)
{
	cout << "wait connect..." << endl;
	//asio程序必须的io_service对象  
	//io_service ios;  
	//创建服务器断点对象，决定服务器到底是接收处理ipv4还是ipv6协议以及接收处理哪个端口号
	ip::tcp::endpoint ep(ip::tcp::v4(), PORT_NUM);

	//用于接收连接的对象  
	ip::tcp::acceptor acceptor(pool.get(), ep);

	//不停的接收连接
	while (true)
	{

		//初始化一个socket对象  
		socket_type sock(new ip::tcp::socket(pool.get()));

		//阻塞，以等待socket连接  
		acceptor.accept(*sock);

		//为每一个建立连接的客户端建立一个线程处理数据  
		thread t(proc_data, sock);
	}
}


int main(int argc, char *argv[])
{

	worker<queue_type> w(q, print_string);
	w.start();

	//创建并发线程池
	boost::shared_ptr<io_service_pool> ios_pool =
		boost::make_shared<io_service_pool>(4);
	//线程接收函数
	proc_accept(*ios_pool);

	//创建线程，处理线程接收函数
	//thread t1(proc_accept);
	//线程阻塞等待，直到线程处理结束  
	//t1.join();

	//阻塞式运行线程池
	ios_pool->run();
	return 0;
};