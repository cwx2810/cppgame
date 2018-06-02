#ifndef _COMM_H_  
#define _COMM_H_  

#include <boost/asio.hpp>  
#include <boost/shared_ptr.hpp>  
#include <boost/make_shared.hpp>  
#include <boost/bind.hpp>  
#include <boost/function.hpp>  
#include <boost/thread.hpp>  
#include <boost/asio/placeholders.hpp>  


#include <boost/date_time/posix_time/posix_time.hpp>  

using namespace boost::asio;

using namespace boost;


#include <string>  
#include <vector>  

#include <iostream>  
using namespace std;

//�˿ں�   
#define PORT_NUM  9999 

//��boost����ָ�����socket���󣬼�����
typedef  boost::shared_ptr<ip::tcp::socket> socket_type;

#endif  

