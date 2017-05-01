#include "event.h"
Event *CreateEvent(Event *evt){
  switch(evt->GetEventName()){
    case copilot::enums::EventName::KeyRelease:
    case copilot::enums::EventName::KeyPress:{
    KeyEvent *key=(KeyEvent*)evt;
    return new KeyEvent(evt->GetEventName(),
      key->shift,
      key->ctrl,
      key->alt,
      key->keyCode,
      key->keyValue
    );
    }
    case copilot::enums::EventName::TextChange:
    {
      TextChangeEvent *textChange = (TextChangeEvent*)evt;
      return new TextChangeEvent(textChange->text);
    }
    default:
    return new Event(evt->GetEventName());
  }
}