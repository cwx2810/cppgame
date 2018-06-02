#include "io_service_pool.h"

//���캯��
io_service_pool::io_service_pool(int n):m_next_io_service(0)
{
	//��֤������һ���̱߳�����
	if (n <= 0)
	{
		n = 1;
	}
	//��������Ĵ�������
	init(n);
}

//�������ioservice����work������
void io_service_pool::init(int n)
{
	for (int i = 0; i < n; ++i)
	{
		//�ù�����������������
		m_io_services.push_back(boost::factory<ios_type*>()());
		m_works.push_back(boost::factory<work_type*>()(m_io_services.back()));
	}
}


//��ѯ��ȡio_service
io_service_pool::ios_type &io_service_pool::get()
{
	//service���ڵ���4��ʱ�����������0����ͷ��ʼ���Ӷ�ʵ������ѯ
	if (m_next_io_service >= m_io_services.size())
	{
		m_next_io_service = 0;
	}
	return m_io_services[m_next_io_service++];
}

//�����¼�ѭ����������
void io_service_pool::start()
{
	//����0˵���Ѿ����߳��ˣ�˵���Ѿ���ʼ��
	if (m_threads.size() > 0)
	{
		return;
	}
	//���û�о��õ������������������¼�ѭ��
	BOOST_FOREACH(ios_type &ios, m_io_services)
	{
		//ʹ���߳�ִ��io_service::run()�����¼�����ѭ��

		//ref��ʽ���߱����� ���������ô��ݣ�����Ļ�������ֵ���ݡ�
		m_threads.create_thread(boost::bind(&ios_type::run, boost::ref(ios)));
	}
}


//����
void io_service_pool::run()
{
	start();
	m_threads.join_all();  //�����ȴ�
}

void io_service_pool::stop()
{
	//�����������io_service�Ĺ���
	m_works.clear();

	//���ε���io_service��stop��ֹͣ����
	std::for_each(m_io_services.begin(), 
		          m_io_services.end(), 
				  boost::bind(&ios_type::stop, _1));
}