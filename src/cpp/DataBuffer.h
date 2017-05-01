#ifndef COPILIT_DATA_BUFFER
#define COPILIT_DATA_BUFFER

#include <mutex>
#include <condition_variable>
#include <iostream>
#include <queue>
using std::queue;
using std::cout;
using std::lock_guard;
using std::mutex;


template <class T,uint size> 
class Buffer
{
public:
  Buffer():buffer(),mtx(),res_mtx(),waitData(),waitNoData(){}
  void setData(T t){
    std::unique_lock<std::mutex> lck(mtx);
    if(buffer.size()==maxSize) waitNoData.wait(lck);
    {
     lock_guard<std::mutex> lck (res_mtx);
     this->buffer.push(t);
    }
    waitData.notify_all();
  }
  T getData(){
    std::unique_lock<std::mutex> lck(mtx);
    if(buffer.size()==0) waitData.wait(lck);
    T d ;
    {
      lock_guard<std::mutex> lck (res_mtx);
    d= this->buffer.front();
    this->buffer.pop();
    }
    waitNoData.notify_all();
    return d;
  }
  void getDataNoBlock(T *t){
    {
      lock_guard<std::mutex> lck (res_mtx);
      if(buffer.size()==0){
        return ;
      }
      *t=this->buffer.front();
      this->buffer.pop();
    }
    waitNoData.notify_all();
  }
  bool hasMoreData(){
    bool hasMore = false;
    {
      lock_guard<std::mutex> lck (res_mtx);
    hasMore = !this->buffer.empty();
    }
    return hasMore;
  }

protected:
  queue<T> buffer;
  std::mutex mtx;
  std::mutex res_mtx;
  std::condition_variable waitData;
  std::condition_variable waitNoData;
  uint maxSize=size;
};
#endif