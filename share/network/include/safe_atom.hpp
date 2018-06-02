#ifndef _SAFE_ATOM_H
#define _SAFE_ATOM_H


//使用共享互斥量实现多线程环境下安全的读写操作
//共享互斥量允许线程获取多个共享所有权和一个专享所有权
//这就实现了读写锁的机制，即多个读线程一个写线程
template<typename T>
class safe_atom
{
	typedef shared_mutex mutex_t;   //共享互斥量
public:
	safe_atom(T v);
	void setValue(const T &v);
	void getValue(T &v);
private:
	mutex_t m_mu;
	T value;
};

template<typename T>
safe_atom<T>::safe_atom(T v):value(v)
{
}

template<typename T>
void safe_atom<T>::setValue(const T &v)
{
	unique_lock<shared_mutex> lock(m_mu);   //写锁定时使用
	value = v;
}

template<typename T>
void safe_atom<T>::getValue(T &v)				
{
	shared_lock<shared_mutex> lock(m_mu);   //读锁定时使用
	v = value;
}

#endif