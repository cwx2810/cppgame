#include <iostream>
using namespace std;
#include "comm.h"

#include "test_tcp_message.h" 
#include "io_service_pool.h"

//�ͻ������ӷ������̴߳�����
void proc_connect(socket_type sock, ip::tcp::endpoint& ep)
{
	//��־λ���ж��Ƿ������Ϸ�����
	bool bConnect = false;
	//�������Ӵ�������
	while (true)
	{
		try
		{
			if (!bConnect)
			{
				cout << "connecting..." << endl;

				//������timer  
				io_service ios;
				deadline_timer timer(ios, posix_time::seconds(1));
				timer.wait();  //����ͬ���ȴ�  
				sock->connect(ep);
			}
			//���ӳɹ�
			else
			{
				//���������������
				string str;
				cout << "input:" << endl;
				cin >> str;


				//sock->write_some(buffer(str));
				//�����ַ����飬��ȡ������Ϣ
				//vector<char> strBuf(100, 0);
				//sock->read_some(buffer(strBuf));
				//cout << "data from server:" << &strBuf[0] << endl;


				//////////////////////////////////////////////////////////////////////////  
				//test tcp_message ʹ��tcp_message�������ݵİ�װ  
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
		//���������쳣
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
	//��ʼ��ios����
	//io_service ios;

	//���������̳߳�
	boost::shared_ptr<io_service_pool> ios_pool =
		boost::make_shared<io_service_pool>(2);

	//��ʼ�����ӷ�������socket  
	socket_type sock(new ip::tcp::socket(ios_pool->get()));
	//��ʼ����Ҫ���ӵķ������ĵ�ַ�Լ��˿ں�  
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), PORT_NUM);
	//�ͻ������ӷ��������߳�
	//thread t1(proc_connect, sock, ep);
	//t1.join();

	proc_connect(sock, ep);
	//����ʽ����
	ios_pool->run();

	return 0;
}