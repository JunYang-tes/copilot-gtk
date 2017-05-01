#ifndef COPILOT_EVENT_DISPATCHER
#define COPILOT_EVENT_DISPATCHER

#include "event.h"
#include <mutex>
#include <condition_variable>
#include <iostream>
class EventDispatcher {
  public:
   void SetEvent(Event *evt);
   Event* GetEvent();
  private:
   Event *event;
   std::mutex mtx;
   std::condition_variable waitEvent;
   std::condition_variable waitUseEvent;
   bool hasEvent=false;
};
#endif