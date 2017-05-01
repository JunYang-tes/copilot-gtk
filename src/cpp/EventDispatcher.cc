#include "EventDispatcher.h"
void EventDispatcher::SetEvent(Event *evt){
  std::unique_lock<std::mutex> lck(mtx);
  if(hasEvent)  waitUseEvent.wait(lck);
  this->event=CreateEvent(evt);
  hasEvent=true;
  waitEvent.notify_all();
}
Event* EventDispatcher::GetEvent(){
  std::unique_lock<std::mutex> lck(mtx);
  if (!hasEvent){
    waitEvent.wait(lck);
  } 
  Event *evt=CreateEvent(this->event);
  delete this->event;
  hasEvent=false;
  waitUseEvent.notify_all();
  return evt;
}