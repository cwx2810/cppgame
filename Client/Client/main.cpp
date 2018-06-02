#include <iostream>
using namespace std;
#include "comm.h"

#include "test_tcp_message.h" 
#include "io_service_pool.h"

//客户端连接服务器线程处理函数
void proc_connect(socket_type sock, ip::tcp::endpoint& ep)
{
	//标志位，判断是否连接上服务器
	bool bConnect = false;
	//不断连接处理数据
	while (true)
	{
		try
		{
			if (!bConnect)
			{
				cout << "connecting..." << endl;

				//阻塞的timer  
				io_service ios;
				deadline_timer timer(ios, posix_time::seconds(1));
				timer.wait();  //进行同步等待  
				sock->connect(ep);
			}
			//连接成功
			else
			{
				//向服务器发送数据
				string str;
				cout << "input:" << endl;
				cin >> str;


				//sock->write_some(buffer(str));
				//创建字符数组，读取回显信息
				//vector<char> strBuf(100, 0);
				//sock->read_some(buffer(strBuf));
				//cout << "data from server:" << &strBuf[0] << endl;


				//////////////////////////////////////////////////////////////////////////  
				//test tcp_message 使用tcp_message进行数据的包装  
				cout << "///////////////////////////////////////////" << endl;
				cout << "send data to server..." << endl;
				send_data(sock, str);
				cout << endl;
				cout << "recv data from server..." << endl;
				read_data(sock);
				cout << "///////////////////////////////////////////" << endl << endl;
			}
			bConnect = true;
		}
		//捕获网络异常
		catch (std::exception& e)
		{
			sock->close();
			bConnect = false;
			cout << e.what() << endl;
		}
	}
}

int main(int argc, char *argv[]) 
{
	//初始化ios对象
	//io_service ios;

	//创建并发线程池
	boost::shared_ptr<io_service_pool> ios_pool =
		boost::make_shared<io_service_pool>(2);

	//初始化连接服务器的socket  
	socket_type sock(new ip::tcp::socket(ios_pool->get()));
	//初始化需要连接的服务器的地址以及端口号  
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), PORT_NUM);
	//客户端连接服务器的线程
	//thread t1(proc_connect, sock, ep);
	//t1.join();

	proc_connect(sock, ep);
	//阻塞式运行
	ios_pool->run();

	return 0;
}