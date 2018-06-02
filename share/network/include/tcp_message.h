#ifndef _TCP_MESSAGE_H  
#define _TCP_MESSAGE_H  

#include "msg_def.h"  
#include <boost/checked_delete.hpp>  //������ָ��ɾ��  
#include <boost/shared_ptr.hpp>      //����ָ��  
#include <boost/smart_ptr.hpp>  
#include <boost/array.hpp>           //���������STL��������װ  
#include <boost/function.hpp>  
#include <boost/noncopyable.hpp>     //��ֹ����  
#include <boost/crc.hpp>             //crcУ��  

using namespace boost;

//class tcp_session;  
//typedef boost::shared_ptr<tcp_session> tcp_session_ptr; //����ָ�붨��  

//Ϊ�˱�����Ϣ�ڴ��ݹ����з��������Ĵ���  
//�Ѹ�����Ƴ�һ�����ɿ����ģ�������ǿ��ֻ����ָ��ķ�ʽʹ�ã�  

class tcp_message : boost::noncopyable
{
public:
	//���������������٣�����Ҫ����������
	typedef boost::function<void(tcp_message *)> destroy_type;         //������  
	typedef char char_type;
	typedef boost::array<char_type, sizeof(msg_head)> head_data_type;  //�����Ϣͷ�����ݣ��̶���С��  
	typedef boost::array<char_type, MAX_MSG_SIZE> body_data_type;      //�����Ϣ������ݣ����ǹ̶���С��  

public:
	//template<typename Func>  
	//tcp_message(const tcp_session_ptr &s, Func func);  
	//tcp_message(const tcp_session_ptr &s);  
	//tcp_message();  
	//tcp_session_ptr get_session();  

	tcp_message();

	template<typename Func>
	tcp_message(Func func);

	void destroy();

	//����ֱ�ӷ������ݵĳ�Ա����  
	head_data_type& head_data();
	body_data_type& msg_data();

	//ת��Ϊ��Ϣͷ�ṹ��Ĳ���  
	msg_head* get_head();

	//�򵥵ļ����Ϣͷ�Ƿ���ȷ  
	bool check_head();

	//�����Ϣ���CRCУ��  
	bool check_msg_crc();
	void set_msg_crc();
private:
	//tcp_session_ptr m_session;  
	destroy_type    m_destroy;//�������ĳ�Ա�����������ﶨ����ܴ�����

	head_data_type m_head;  //��Ա�����м�����Ϣͷ  
	body_data_type m_msg;   //������Ϣ��  

};

//��Щ��һЩ�������ȷ��������ʹ�õ�����  
typedef tcp_message   tcp_request;       //tcp������Ϣ  
typedef tcp_message   tcp_response;      //tcp��Ӧ��Ϣ   
typedef tcp_request*  tcp_request_ptr;   //tcp������Ϣ��ָ��  
typedef tcp_response* tcp_response_ptr;  //tcp��Ӧ��Ϣ��ָ��  


#endif  