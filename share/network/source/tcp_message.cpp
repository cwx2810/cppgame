#include "tcp_message.h"  

//   
// template<typename Func>  
// tcp_message::tcp_message(const tcp_session_ptr &s, Func func):  
// m_session(s), m_destroy(func)  
// {  
//   
// }  
//   
//   
// tcp_message::tcp_message(const tcp_session_ptr &s):  
// m_session(s)  
// {  
//   
// }  
//   
//   
// tcp_session_ptr tcp_message::get_session()  
// {  
//  return m_session;  
// }  



tcp_message::tcp_message()
{

}


template<typename Func>
tcp_message::tcp_message(Func func)
	:m_destroy(func)
{

}

//销毁消息  
void tcp_message::destroy()
{
	//如果传入的销毁器，就使用传入的销毁器进行销毁消息  
	if (m_destroy)
	{
		m_destroy(this);
	}
	else
	{
		//使用boost中的销毁器销毁自身  
		boost::checked_delete(this);
	}
}

//获取消息头数据  
tcp_message::head_data_type &tcp_message::head_data()
{
	return m_head;
}

//获取消息体数据  
tcp_message::body_data_type &tcp_message::msg_data()
{
	return m_msg;
}

//将消息头的数据转换为消息体的结构  
msg_head* tcp_message::get_head()
{
	return reinterpret_cast<msg_head *>(m_head.data());
}

//简单地检查消息头是否正确  
bool tcp_message::check_head()
{
	return (get_head()->size < MAX_MSG_SIZE);
}

//检查消息体的crc校验  
bool tcp_message::check_msg_crc()
{
	//使用boost的crc库进行简单的校验  
	boost::crc_32_type crc32;

	//计算校验和  
	crc32.process_bytes(&m_msg[0], get_head()->size);

	//比较crc值  
	return get_head()->chksum == crc32.checksum();
}

//在消息头里设置消息体的crc校验  
void tcp_message::set_msg_crc()
{
	boost::crc_32_type crc32;

	//计算校验和  
	crc32.process_bytes(&m_msg[0], get_head()->size);

	//设置校验  
	get_head()->chksum = crc32.checksum();
}