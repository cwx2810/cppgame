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
	//��Ϣͷ  
	msg_head head;
	head.size = str.length() + 1;
	head.chksum = std::for_each(str.begin(), str.end(), crc_32_type())();
	head.type = 99;

	//������Ϣ��  
	boost::shared_ptr<tcp_message> pSendMsg = boost::make_shared<tcp_message>();
	pSendMsg->get_head()->chksum = head.chksum;
	pSendMsg->get_head()->size = head.size;
	pSendMsg->get_head()->type = head.type;

	//��������  
	//std::copy(str.begin(), str.end(), pSendMsg->msg_data().begin());  

	memcpy(pSendMsg->msg_data().begin(), str.c_str(), head.size);


	//����У���  
	pSendMsg->set_msg_crc();

	//��������  

	//�ȷ�����Ϣͷ  
	sock->write_some(buffer(pSendMsg->head_data().data(), pSendMsg->head_data().size()));

	//�ٷ�����Ϣ��  
	sock->write_some(buffer(pSendMsg->msg_data().data(), pSendMsg->get_head()->size));

	int n = pSendMsg->get_head()->size;
	//���  
	print_msg(pSendMsg);

}


std::string  read_data(socket_type sock)
{
	//������Ϣ��  
	boost::shared_ptr<tcp_message> pRecvMsg = boost::make_shared<tcp_message>();

	//����Ϣͷ  
	sock->read_some(buffer(pRecvMsg->head_data().data(), pRecvMsg->head_data().size()));

	//����Ϣ��  
	sock->read_some(buffer(pRecvMsg->msg_data().data(), pRecvMsg->get_head()->size));


	//У��  
	bool bRight = pRecvMsg->check_msg_crc();

	//�������  
	if (bRight)
	{
	//	print_msg(pRecvMsg);
	}
	return pRecvMsg->msg_data().data();
}



#endif  