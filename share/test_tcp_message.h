#ifndef _TEST_TCPMESSAGE_H  
#define _TEST_TCPMESSAGE_H  

#include "comm.h"  
#include "tcp_message.h"  


void print_msg(boost::shared_ptr<tcp_message> pMsg)
{
	cout << "head type:" << pMsg->get_head()->type << endl;
	cout << "head size:" << pMsg->get_head()->size << endl;
	cout << "head checksum:" << pMsg->get_head()->chksum << endl;
	cout << "data:" << pMsg->msg_data().data() << endl;
}


//test tcp_message  
void send_data(socket_type sock, const std::string& str)
{
	//消息头  
	msg_head head;
	head.size = str.length() + 1;
	head.chksum = std::for_each(str.begin(), str.end(), crc_32_type())();
	head.type = 99;

	//构建消息体  
	boost::shared_ptr<tcp_message> pSendMsg = boost::make_shared<tcp_message>();
	pSendMsg->get_head()->chksum = head.chksum;
	pSendMsg->get_head()->size = head.size;
	pSendMsg->get_head()->type = head.type;

	//拷贝数据  
	//std::copy(str.begin(), str.end(), pSendMsg->msg_data().begin());  

	memcpy(pSendMsg->msg_data().begin(), str.c_str(), head.size);


	//设置校验和  
	pSendMsg->set_msg_crc();

	//阻塞发送  

	//先发送消息头  
	sock->write_some(buffer(pSendMsg->head_data().data(), pSendMsg->head_data().size()));

	//再发送消息体  
	sock->write_some(buffer(pSendMsg->msg_data().data(), pSendMsg->get_head()->size));

	int n = pSendMsg->get_head()->size;
	//输出  
	print_msg(pSendMsg);

}


std::string  read_data(socket_type sock)
{
	//创建消息体  
	boost::shared_ptr<tcp_message> pRecvMsg = boost::make_shared<tcp_message>();

	//读消息头  
	sock->read_some(buffer(pRecvMsg->head_data().data(), pRecvMsg->head_data().size()));

	//读消息体  
	sock->read_some(buffer(pRecvMsg->msg_data().data(), pRecvMsg->get_head()->size));


	//校验  
	bool bRight = pRecvMsg->check_msg_crc();

	//输出数据  
	if (bRight)
	{
	//	print_msg(pRecvMsg);
	}
	return pRecvMsg->msg_data().data();
}



#endif  