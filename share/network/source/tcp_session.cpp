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
	
	//表示有一个会话端连接
	//if (m_connect_func)
	//{
	////	m_connect_func(shared_from_this());
	//}
	//说明该会话已连接
	setconnected(true);
	read_data();
}


void tcp_session::read_data()
{
	//step3-->创建消息对象
	tcp_request_ptr req = create_request();

	//启动异步读取
	read(req);
}

//使用内存池创建tcp_message对象
tcp_request_ptr tcp_session::create_request()
{
	//此处将tcp_session与tcp_message绑定在一起
	return m_msg_pool.construct(shared_from_this());
}


void tcp_session::setconnected(bool bFlag)
{
	m_bconnected.setValue(bFlag);
}

//关闭TCP连接
void tcp_session::close()
{
	//当当前连接状态变为断开状态时调用断开连接的回调函数
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


//启动异步读
void tcp_session::read(tcp_request_ptr req)
{
	//step4-->异步读取消息头
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


//异步读取消息头
void tcp_session::handle_read_head(const boost::system::error_code &error, 
								   size_t bytes_transferred, 
								   tcp_request_ptr req)
{
	//检查消息头是否正确，消息头正确，再读取消息体
	if (error || !req->check_head())
	{
		close();
		return; 
	}

	//step5-->异步读取消息体
	async_read(m_socket, 
		       buffer(req->msg_data().data(), req->get_head()->size),
		       m_strand.wrap(
		                     bind(&tcp_session::handle_read_msg,
		                           shared_from_this(),
		                           placeholders::error, 
		                           placeholders::bytes_transferred,
		                           req)));
}

//异步读取消息
void tcp_session::handle_read_msg(const boost::system::error_code &error, 
								  size_t byte_transferred, 
								  tcp_request_ptr req)
{
	bool bRight = req->check_msg_crc();

	//检查消息体是否正确
	if (error || !bRight)
	{
		close();
		return;
	}


	//step6-->将收到的消息加入到job_queue
	//m_queue是从外部传来的消息队列
	//此处是将该连接读取的所有数据都保存在该队列中
	//类似于生产者
	m_queue.push(req);

	//启动新的异步读
	read_data();
}


bool tcp_session::isconnected()
{
	bool bFlag = false;
	m_bconnected.getValue(bFlag);
	return bFlag;
}


//启动异步写
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

	//消息发送完毕以后，就应该释放内存
	resq->destroy();
}





//void tcp_session::set_callbackfunc(function_type &connfunc, function_type &disconnfunc)
//{
//	m_connect_func    = connfunc;
//	m_disconnect_func = disconnfunc;
//}