#include "tcp_session.hpp"
#include <boost/bind.hpp>
#include <iostream>
#include <string>

using namespace std;

#include <boost/bind.hpp>
#include <boost/ref.hpp>

using namespace boost;
using namespace boost::asio;


tcp_session::object_pool_type tcp_session::m_msg_pool;

tcp_session::tcp_session(ios_type &ios, queue_type &q):
m_socket(ios), m_strand(ios), m_queue(q), m_bconnected(false)
{
}


tcp_session::socket_type &tcp_session::socket()
{
	return m_socket;
}

tcp_session::ios_type &tcp_session::io_service()
{
	return m_socket.get_io_service();
}

void tcp_session::start()
{
	
	//��ʾ��һ���Ự������
	//if (m_connect_func)
	//{
	////	m_connect_func(shared_from_this());
	//}
	//˵���ûỰ������
	setconnected(true);
	read_data();
}


void tcp_session::read_data()
{
	//step3-->������Ϣ����
	tcp_request_ptr req = create_request();

	//�����첽��ȡ
	read(req);
}

//ʹ���ڴ�ش���tcp_message����
tcp_request_ptr tcp_session::create_request()
{
	//�˴���tcp_session��tcp_message����һ��
	return m_msg_pool.construct(shared_from_this());
}


void tcp_session::setconnected(bool bFlag)
{
	m_bconnected.setValue(bFlag);
}

//�ر�TCP����
void tcp_session::close()
{
	//����ǰ����״̬��Ϊ�Ͽ�״̬ʱ���öϿ����ӵĻص�����
	//if (isconnected())
	//{
	//	if (m_disconnect_func)
	//	{
	//		m_disconnect_func(shared_from_this());
	//	}
	//}

	setconnected(false);
	m_socket.close();
}


//�����첽��
void tcp_session::read(tcp_request_ptr req)
{
	//step4-->�첽��ȡ��Ϣͷ
	async_read(m_socket, 
		       buffer(req->head_data().data(),
			          req->head_data().size()),
			   m_strand.wrap(
			                 bind(&tcp_session::handle_read_head,
							 shared_from_this(),
							 placeholders::error, 
							 placeholders::bytes_transferred,
							 req)));
}


//�첽��ȡ��Ϣͷ
void tcp_session::handle_read_head(const boost::system::error_code &error, 
								   size_t bytes_transferred, 
								   tcp_request_ptr req)
{
	//�����Ϣͷ�Ƿ���ȷ����Ϣͷ��ȷ���ٶ�ȡ��Ϣ��
	if (error || !req->check_head())
	{
		close();
		return; 
	}

	//step5-->�첽��ȡ��Ϣ��
	async_read(m_socket, 
		       buffer(req->msg_data().data(), req->get_head()->size),
		       m_strand.wrap(
		                     bind(&tcp_session::handle_read_msg,
		                           shared_from_this(),
		                           placeholders::error, 
		                           placeholders::bytes_transferred,
		                           req)));
}

//�첽��ȡ��Ϣ
void tcp_session::handle_read_msg(const boost::system::error_code &error, 
								  size_t byte_transferred, 
								  tcp_request_ptr req)
{
	bool bRight = req->check_msg_crc();

	//�����Ϣ���Ƿ���ȷ
	if (error || !bRight)
	{
		close();
		return;
	}


	//step6-->���յ�����Ϣ���뵽job_queue
	//m_queue�Ǵ��ⲿ��������Ϣ����
	//�˴��ǽ������Ӷ�ȡ���������ݶ������ڸö�����
	//������������
	m_queue.push(req);

	//�����µ��첽��
	read_data();
}


bool tcp_session::isconnected()
{
	bool bFlag = false;
	m_bconnected.getValue(bFlag);
	return bFlag;
}


//�����첽д
void tcp_session::write(tcp_response_ptr resp)
{
	async_write(m_socket,
		        buffer(resp->head_data().data(), resp->head_data().size()),
				m_strand.wrap(
				              bind(&tcp_session::handle_write_head, 
							  shared_from_this(), 
							  placeholders::error, 
							  placeholders::bytes_transferred,
							  resp)));
}



void tcp_session::handle_write_head(const boost::system::error_code &error, 
									size_t bytes_transferred, 
									tcp_response_ptr resp)
{
	if (error || bytes_transferred != resp->head_data().size())
	{
		close();
		return;
	}


	async_write(m_socket,
		        buffer(resp->msg_data().data(), resp->get_head()->size),
				m_strand.wrap(
				              bind(&tcp_session::handle_write_msg,
							        shared_from_this(),
									placeholders::error,
									placeholders::bytes_transferred,
									resp)));
}

void tcp_session::handle_write_msg(const boost::system::error_code &error, 
								   size_t byte_transferred, 
								   tcp_response_ptr resq)
{
	if (error || byte_transferred != resq->get_head()->size)
	{
		close();
		return;
	}

	//��Ϣ��������Ժ󣬾�Ӧ���ͷ��ڴ�
	resq->destroy();
}





//void tcp_session::set_callbackfunc(function_type &connfunc, function_type &disconnfunc)
//{
//	m_connect_func    = connfunc;
//	m_disconnect_func = disconnfunc;
//}