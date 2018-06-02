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

//������յ�������
void proc_data(socket_type sock)
{
	//����ͻ��˵ĵ�ַ
	cout << "client connected :" << sock->remote_endpoint().address() << endl;

	//��ͣ�Ĵ���
	while (true)
	{
		//������ܷ����������쳣������ͻ���ͻȻ�Ͽ�  
		try
		{
			//ʹ���ַ����飬��Ϊ����ײ����ݶ����ַ�����������100������ʼ��Ϊ0
			//vector<char> str(100, 0);

			//��ȡ�ͻ��˷��͵�����  
			//ͬ�����ƣ�������ȡ����  
			//sock->read_some(buffer(str));
			//cout << "recv from:" << sock->remote_endpoint().address() << endl;
			//ͨ��ȡ�׵�ַ�ķ���ȡ����ȡ������
			//cout << "data:" << &str[0] << endl;
			//cout << endl;

			//���ͻ��˷��͵����ݻ��Ը��ͻ���  
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
			//����ѭ�����Ͽ�����
			break;
		}
	}
}


//�߳̽��պ��� 
void proc_accept(io_service_pool &pool)
{
	cout << "wait connect..." << endl;
	//asio��������io_service����  
	//io_service ios;  
	//�����������ϵ���󣬾��������������ǽ��մ���ipv4����ipv6Э���Լ����մ����ĸ��˿ں�
	ip::tcp::endpoint ep(ip::tcp::v4(), PORT_NUM);

	//���ڽ������ӵĶ���  
	ip::tcp::acceptor acceptor(pool.get(), ep);

	//��ͣ�Ľ�������
	while (true)
	{

		//��ʼ��һ��socket����  
		socket_type sock(new ip::tcp::socket(pool.get()));

		//�������Եȴ�socket����  
		acceptor.accept(*sock);

		//Ϊÿһ���������ӵĿͻ��˽���һ���̴߳�������  
		thread t(proc_data, sock);
	}
}


int main(int argc, char *argv[])
{

	worker<queue_type> w(q, print_string);
	w.start();

	//���������̳߳�
	boost::shared_ptr<io_service_pool> ios_pool =
		boost::make_shared<io_service_pool>(4);
	//�߳̽��պ���
	proc_accept(*ios_pool);

	//�����̣߳������߳̽��պ���
	//thread t1(proc_accept);
	//�߳������ȴ���ֱ���̴߳������  
	//t1.join();

	//����ʽ�����̳߳�
	ios_pool->run();
	return 0;
};