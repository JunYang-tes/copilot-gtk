#include <nan.h>
// #include "binding.h"
#include <node.h>
#include <uv.h>
#include <iostream>
using std::cout;
#include <iostream>
#include <thread>
#include <string>
#include "gui.h"
#include "event.h"
#include "EventDispatcher.h"
#include "data.h"

using namespace v8;
using copilot::enums::EventName;
using Nan::Utf8String;

struct Baton
{
    uv_work_t                    request;
    v8::Persistent<v8::Function> callback;
    int argc;
    Persistent<Value> argv;
    Event *evt;
};

EventDispatcher eventDispatcher;
std::thread *guiThread=NULL;
void show(const FunctionCallbackInfo<Value>& args){
  Show();
}
void hide(const FunctionCallbackInfo<Value>& args){
  Hide();
  cout<<"Hide\n";
}
void display(const FunctionCallbackInfo<Value>& args){
  auto obj = args[0].As<Object>();
  auto type = Nan::Get(obj, Nan::New("type").ToLocalChecked()).ToLocalChecked();
  if (!type->IsString())
  {
    Nan::ThrowError("Can only invoke gui once");
    return;
  }
  std::string typeStr = std::string(*Utf8String(type));

  vector<ListItemData> items;
  if(typeStr.compare("list")==0){
    auto data = Nan::Get(obj,Nan::New("data").ToLocalChecked()).ToLocalChecked();
    if(!data->IsArray()){
      Nan::ThrowError(Nan::TypeError("Need array as data"));
    }
    auto array = data.As<v8::Array>();
    auto len = array->Length();    
    for(uint i=0;i<len;i++){
      auto item =Nan::Get(array,i).ToLocalChecked().As<Object>();
      items.push_back(ListItemData(
        string(* Utf8String(Nan::Get(item,Nan::New("title").ToLocalChecked()).ToLocalChecked())),
        string(* Utf8String(Nan::Get(item,Nan::New("text").ToLocalChecked()).ToLocalChecked())),
        string(* Utf8String(Nan::Get(item,Nan::New("icon").ToLocalChecked()).ToLocalChecked()))
      ));
    }
    DisplayList(items);
  }
}

static void AsyncWork (uv_work_t* req);
static void AsyncAfter(uv_work_t* req,int status);
void startGUI();
void AsyncGUI(const v8::FunctionCallbackInfo<Value>&  args) 
{
  if(guiThread){
    Nan::ThrowError("Can only invoke gui once");
    return ;
  }
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  if (args.Length() != 1) {
    Nan::ThrowError("Need callback");
    return ;
  }

  if (!args[0]->IsFunction()) {
    Nan::ThrowError("Argument is not a callback");
    return ;
  }

  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);
  Baton* baton = new Baton();
  baton->request.data = baton;
  baton->callback.Reset(isolate,callback);
  guiThread =new std::thread(startGUI);
  uv_queue_work(uv_default_loop(), &baton->request, AsyncWork, AsyncAfter);
}


static void AsyncWork (uv_work_t* req)
{
  Baton* baton = static_cast<Baton*>(req->data);
  baton->evt=eventDispatcher.GetEvent();
}

static void AsyncAfter(uv_work_t* req,int status)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);
    Baton* baton = static_cast<Baton*>(req->data);
    Event *evt = baton->evt;  
  void dispatchGUIEvent(Event *evt,Baton *baton);
  dispatchGUIEvent(evt,baton);
    delete evt;
    Local<Value> argv[]={ baton->argv.Get(isolate)};
    v8::TryCatch try_catch(isolate);
    baton->callback.Get(isolate)->Call(isolate->GetCurrentContext()->Global(), 1,argv);
    if (try_catch.HasCaught()) {
        // node::FatalException(try_catch);
    }
    uv_queue_work(uv_default_loop(), &baton->request, AsyncWork, AsyncAfter);
}
void guiEvent(void *ctx,Event *evt){
  eventDispatcher.SetEvent(evt);
}
void onKeyRelease(KeyEvent *evt,Baton *baton){
  Isolate* isolate = Isolate::GetCurrent();

  v8::Local<v8::Object> arg = Nan::New<v8::Object>();
    Nan::Set(arg, Nan::New("type").ToLocalChecked(), Nan::New("key-release").ToLocalChecked());
    v8::Local<v8::Object> event = Nan::New<v8::Object>();
    Nan::Set(arg, Nan::New("event").ToLocalChecked(), event);
    Nan::Set(event, Nan::New("keyCode").ToLocalChecked(), Nan::New(evt->keyCode));
    Nan::Set(event, Nan::New("keyValue").ToLocalChecked(), Nan::New(evt->keyValue).ToLocalChecked());
    Nan::Set(event, Nan::New("shift").ToLocalChecked(), Nan::New(evt->shift));
    Nan::Set(event, Nan::New("ctrl").ToLocalChecked(), Nan::New(evt->ctrl));
    Nan::Set(event, Nan::New("alt").ToLocalChecked(), Nan::New(evt->alt));
    baton->argv.Reset(isolate,arg);
}
void onTextChange(TextChangeEvent *evt,Baton *baton){
  Isolate* isolate = Isolate::GetCurrent();
    Local<Object> arg = Nan::New<v8::Object>();
    Nan::Set(arg, Nan::New("type").ToLocalChecked(), Nan::New("text-change").ToLocalChecked());
    v8::Local<v8::Object> event = Nan::New<v8::Object>();
    Nan::Set(arg, Nan::New("event").ToLocalChecked(), event);
    Nan::Set(event, Nan::New("text").ToLocalChecked(), Nan::New(evt->text.c_str()).ToLocalChecked());
    baton->argv.Reset(isolate,arg);
}
void onQuit(Baton *baton){
  Isolate * isolate = Isolate::GetCurrent();
  Local<Object> arg = Nan::New<v8::Object>();
    Nan::Set(arg, Nan::New("type").ToLocalChecked(), Nan::New("quit").ToLocalChecked());
  baton->argv.Reset(isolate,arg);
}

void dispatchGUIEvent(Event *evt,Baton *baton){
    switch (evt->GetEventName())
    {
    case copilot::enums::EventName::KeyRelease:
       onKeyRelease((KeyEvent *)evt,baton);
       break;
    case EventName::TextChange:
       onTextChange((TextChangeEvent *)evt,baton);
       break;
    case EventName::Quit:
      onQuit(baton);
      break;
    default:
      cout<<"Unsupported GUI Event\n";
       break;
    }
}

void startGUI(){
  RegsterEventHandler(copilot::enums::EventName::KeyRelease,
                        EventCallbackInfo((EventCallback)guiEvent, NULL));
  RegsterEventHandler(copilot::enums::EventName::TextChange,
                        EventCallbackInfo((EventCallback)guiEvent, NULL));
  EntryPoint();
  Event quit(EventName::Quit);
  eventDispatcher.SetEvent(&quit);
}


void init(Handle<Object> exports) 
{
  Isolate* isolate = Isolate::GetCurrent();
  exports->Set(String::NewFromUtf8(isolate, "gui"),
      FunctionTemplate::New(isolate, AsyncGUI)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate,"display"),
      FunctionTemplate::New(isolate,display)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate,"show"),
      FunctionTemplate::New(isolate,show)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate,"hide"),
      FunctionTemplate::New(isolate,hide)->GetFunction());
  
}
NODE_MODULE(gui, init)
