#ifndef COPILOT_EVENT_H
#define COPILOT_EVENT_H
#include <string>
using std::string;
namespace copilot{
  namespace enums {
    enum EventName {
      KeyRelease,
      KeyPress,
      TextChange,
      Quit
    };
  };
};
class Event {
  public:
    Event(){}
    Event(copilot::enums::EventName t):name(t){}
    copilot::enums::EventName GetEventName(){
      return name;
    }
  protected:
    copilot::enums::EventName name;
};
class KeyEvent:public Event {
  public:

    KeyEvent(
      copilot::enums::EventName event,
      bool shift,bool ctrl,bool alt,int keyCode,string keyValue)
    :Event(event),
    shift(shift),ctrl(ctrl),alt(alt),keyCode(keyCode),keyValue(keyValue)
    
    {}
    
    bool shift = false;
    bool ctrl = false;
    bool alt = false;
    int keyCode;
    string keyValue;
};


class TextChangeEvent:public Event {
  public:
    TextChangeEvent(string t):Event(copilot::enums::EventName::TextChange),text(t){
    }
    std::string text;
};
typedef void (*EventCallback) (void *context,Event *event);
class EventCallbackInfo {
 public:
  EventCallbackInfo(const EventCallbackInfo &eventCallbackInfo){
    this->context_ = eventCallbackInfo.context_;
    this->callback_ = eventCallbackInfo.callback_;
  }
  EventCallbackInfo(EventCallback callback,void *context):
  callback_(callback),
  context_(context){}

  void Call(Event *event){
    this->callback_(this->context_,event);
  }

 private:
  EventCallback callback_;
  void *context_;
};
Event *CreateEvent(Event *evt);
#endif