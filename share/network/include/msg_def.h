#ifndef _MSGDEF_H  
#define _MSGDEF_H  

#include <boost/cstdint.hpp>  
#include <iostream>  
#include <string>  
using namespace std;

//��ȻC/C++�ṩ�����õ��������Ͷ��壬����ͬ�ļ����Ӳ����ϵ�������Ŀ���ǲ�һ����  
//32λ--��int 4�ֽ�  
//64      int 8�ֽ�  
//Ϊ��ʹ�����Ժ󿪷��ĳ�����п�ƽ̨�ļ����ԣ���������ʹ��Boost�ṩ�ľ�ȷ��ȵ���������  
//��ȷ��������Ķ���  
//ʹ��using�����Լ������ֿռ�  
using boost::int8_t;
using boost::uint8_t;
using boost::int16_t;
using boost::uint16_t;
using boost::int32_t;
using boost::uint32_t;
using boost::int64_t;
using boost::uint64_t;

typedef boost::uint8_t byte_t;
typedef boost::uint8_t uchar_t;
typedef unsigned short ushort_t;
typedef unsigned int   uint_t;
typedef unsigned long  ulong_t;

typedef boost::uint16_t word_t;
typedef boost::uint32_t dword_t;



//������Ϣͷ�Ľṹ  
struct msg_head
{
	uint32_t type;   //��Ϣ����    ����  
	uint32_t size;   //��Ϣ���С  ����  
	uint32_t chksum; //CRCУ��     ����  
};


//�����Ϣ�ĳ��ȴ��� MAX_MSG_SIZE  
//���Էֽ�����  
#define MAX_MSG_SIZE 1024  //��Ϣ����󳤶�  


#endif  
