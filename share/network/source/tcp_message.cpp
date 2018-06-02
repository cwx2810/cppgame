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

//������Ϣ  
void tcp_message::destroy()
{
	//������������������ʹ�ô��������������������Ϣ  
	if (m_destroy)
	{
		m_destroy(this);
	}
	else
	{
		//ʹ��boost�е���������������  
		boost::checked_delete(this);
	}
}

//��ȡ��Ϣͷ����  
tcp_message::head_data_type &tcp_message::head_data()
{
	return m_head;
}

//��ȡ��Ϣ������  
tcp_message::body_data_type &tcp_message::msg_data()
{
	return m_msg;
}

//����Ϣͷ������ת��Ϊ��Ϣ��Ľṹ  
msg_head* tcp_message::get_head()
{
	return reinterpret_cast<msg_head *>(m_head.data());
}

//�򵥵ؼ����Ϣͷ�Ƿ���ȷ  
bool tcp_message::check_head()
{
	return (get_head()->size < MAX_MSG_SIZE);
}

//�����Ϣ���crcУ��  
bool tcp_message::check_msg_crc()
{
	//ʹ��boost��crc����м򵥵�У��  
	boost::crc_32_type crc32;

	//����У���  
	crc32.process_bytes(&m_msg[0], get_head()->size);

	//�Ƚ�crcֵ  
	return get_head()->chksum == crc32.checksum();
}

//����Ϣͷ��������Ϣ���crcУ��  
void tcp_message::set_msg_crc()
{
	boost::crc_32_type crc32;

	//����У���  
	crc32.process_bytes(&m_msg[0], get_head()->size);

	//����У��  
	get_head()->chksum = crc32.checksum();
}