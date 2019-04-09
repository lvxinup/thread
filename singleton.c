//这是一个单例模式的练习代码

#include<iostream>
#include<mutex>

//饿汉模式
template <typename T>
class singleton1
{
  static T data;
public:
  static T* GetInstance()
  {
    return &data;
  }
};

//线程安全的懒汉模式
template <typename T>
class singleton2
{
  volatile static T* inst;//需要设置volatile关键字，否则可能被编译器优化
  static std::mutex lock;
public:
  static T* GetInstance()
  {
    if(inst==nullptr) //双重判定空指针，降低锁冲突的概率，提高性能
    {
      lock.lock();  //使用互斥锁，保证多线程情况下也只调用一次new
      if(inst==nullptr)
      {
        inst = new T();
      }
      lock.unlock();
    }
    return inst;
  }
};
