#include "tcp_client.hpp"

// tcp_client::tcp_client(tcp_type::endpoint ep,
// 					   net_comm *net, int n):
// tcp_comm(net, n),
// m_endpoint(ep),
// m_session(factory<tcp_session_ptr>()(m_ios_pool.get(), net->recv_queue()))
// {
// 	
// 	m_schedule.add(boost::bind(&tcp_client::start_connect, this), 1);
// 	//m_session->set_callbackfunc(m_connectfunc, m_disconnectfunc);
// 	m_schedule.start();
// }


tcp_client::tcp_client(tcp_type::endpoint ep, queue_type& q, int n )
:tcp_comm(q, n), m_endpoint(ep), m_session(factory<tcp_session_ptr>()(m_ios_pool.get(), q))
{
	m_schedule.add(boost::bind(&tcp_client::start_connect, this), 1);
	m_schedule.start();
}


void tcp_client::start_connect()
{
	if (m_session->isconnected())
	{
		return;
	}

	std::cout<<"try to connect to server..."<<std::endl;

	m_session->socket().async_connect(
		m_endpoint,
		boost::bind(&tcp_client::handle_connect,
		this,
		boost::asio::placeholders::error, m_session));
}


void tcp_client::handle_connect(const boost::system::error_code &error, 
								tcp_session_ptr session)
{
	if (error)
	{
		session->close();
		return;
	}

	cout<<"server connected..."<<endl;
	//�ûỰ�ǿͻ�����������������һ���Ự
	session->start();
}