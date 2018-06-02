#ifndef _MSGDEF_H  
#define _MSGDEF_H  

#include <boost/cstdint.hpp>  
#include <iostream>  
#include <string>  
using namespace std;

//虽然C/C++提供了内置的整数类型定义，但不同的计算机硬件体系下整数的宽度是不一样的  
//32位--》int 4字节  
//64      int 8字节  
//为了使我们以后开发的程序具有跨平台的兼容性，所以我们使用Boost提供的精确宽度的整数定义  
//精确宽度整数的定义  
//使用using引入自己的名字空间  
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



//定义消息头的结构  
struct msg_head
{
	uint32_t type;   //消息类型    整数  
	uint32_t size;   //消息体大小  整数  
	uint32_t chksum; //CRC校验     整数  
};


//如果消息的长度大于 MAX_MSG_SIZE  
//可以分解数据  
#define MAX_MSG_SIZE 1024  //消息体最大长度  


#endif  
