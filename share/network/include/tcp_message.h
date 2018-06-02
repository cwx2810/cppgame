#ifndef _TCP_MESSAGE_H  
#define _TCP_MESSAGE_H  

#include "msg_def.h"  
#include <boost/checked_delete.hpp>  //待检查的指针删除  
#include <boost/shared_ptr.hpp>      //智能指针  
#include <boost/smart_ptr.hpp>  
#include <boost/array.hpp>           //关于数组的STL容器风格包装  
#include <boost/function.hpp>  
#include <boost/noncopyable.hpp>     //禁止拷贝  
#include <boost/crc.hpp>             //crc校验  

using namespace boost;

//class tcp_session;  
//typedef boost::shared_ptr<tcp_session> tcp_session_ptr; //共享指针定义  

//为了避免消息在传递过程中反复拷贝的代价  
//把该类设计成一个不可拷贝的，这样就强制只能以指针的方式使用；  

class tcp_message : boost::noncopyable
{
public:
	//无论怎样都能销毁，所以要定义销毁器
	typedef boost::function<void(tcp_message *)> destroy_type;         //销毁器  
	typedef char char_type;
	typedef boost::array<char_type, sizeof(msg_head)> head_data_type;  //存放消息头的数据（固定大小）  
	typedef boost::array<char_type, MAX_MSG_SIZE> body_data_type;      //存放消息体的数据（不是固定大小）  

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

	//可以直接访问数据的成员函数  
	head_data_type& head_data();
	body_data_type& msg_data();

	//转换为消息头结构体的操作  
	msg_head* get_head();

	//简单的检查消息头是否正确  
	bool check_head();

	//检查消息体的CRC校验  
	bool check_msg_crc();
	void set_msg_crc();
private:
	//tcp_session_ptr m_session;  
	destroy_type    m_destroy;//销毁器的成员变量，在这里定义就能传入了

	head_data_type m_head;  //成员变量中加入消息头  
	body_data_type m_msg;   //加入消息体  

};

//这些是一些意义更明确，更便于使用的名称  
typedef tcp_message   tcp_request;       //tcp请求消息  
typedef tcp_message   tcp_response;      //tcp响应消息   
typedef tcp_request*  tcp_request_ptr;   //tcp请求消息的指针  
typedef tcp_response* tcp_response_ptr;  //tcp响应消息的指针  


#endif  