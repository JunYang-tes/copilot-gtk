#include "gui.h"
#include "DataBuffer.h"
#include "event.h"
#include "widgets/ListItem.h"
#include "widgets/RichTextView.h"
#include "EventDispatcher.h"
#include "widgets/Parser.h"
#include "data.h"
#include "widgets/ListView.h"
#include <gtkmm.h>
#include <string>
#include <map>
#include <keybinder.h>
#include <iostream>
using std::map;
using std::string;

using copilot::enums::EventName;
map<EventName, EventCallbackInfo *> eventMap;

using std::cout;
using std::endl;

using Gtk::Application;
using Gtk::Window;
using Gtk::ScrolledWindow;
using Gtk::Box;
using Gtk::Entry;
using sigc::mem_fun;
using Gtk::CssProvider;
using Gdk::Screen;
using Gtk::ListBox;

class CopilotGUI
{

public:
  CopilotGUI():dispatcher()
  {
    app = Gtk::Application::create("copilot.gui",
                                   Gio::ApplicationFlags::APPLICATION_FLAGS_NONE);
    Gdk::Geometry geo;
    geo.max_width = 800;
    window = new Window();
    window->property_default_width().set_value(800);
    window->property_deletable ().set_value(false);
    window->set_skip_taskbar_hint ();
    window->stick();
    window->set_keep_above();
    window->set_resizable(false);
    window->set_title("copilot");

    box = new Box(Gtk::Orientation::ORIENTATION_VERTICAL);
    entry = new Entry();
    window->add(*box);
    window->signal_focus_out_event ().connect(sigc::mem_fun(*this,
      &CopilotGUI::onBlur));
    box->add(*entry);

    list = new ListView();
    window->set_geometry_hints(*list, geo, Gdk::HINT_MAX_SIZE);
    box->pack_start(*list, true, true, 0);
    
    entry->signal_changed().connect(sigc::mem_fun(*this,
                                                  &CopilotGUI::onTextChanged));
    entry->signal_key_release_event().connect(sigc::mem_fun(*this,
                                                            &CopilotGUI::onKeyRelease));
    entry->signal_show().connect(sigc::mem_fun(*this,
      &CopilotGUI::resetGeometry));
    auto css = Gtk::CssProvider::create();
    auto screen = Gdk::Screen::get_default();

    dispatcher.connect(sigc::mem_fun(*this, &CopilotGUI::onReadyToDisplay));

    try
    {
      if (css->load_from_path("./assets/style.css"))
      {
        Gtk::StyleContext::add_provider_for_screen(screen, css, 800);
        // Gtk::STYLE_PROVIDER_PRIORITY_USER);
      }
    }
    catch (Gtk::CssProviderError e)
    {
#ifdef DEBUG
      cout << e.what() << endl;
#endif
    }
  }
  ~CopilotGUI()
  {
    delete entry;
    delete box;
    delete window;
    delete list;
  }
  void show(){
    window->deiconify();
  }
  void resetGeometry(){
    int screen_width=window->get_screen ()->get_width();
    int w,h;
    window->get_size(w,h);
    window->move((screen_width-w)/2,100);
  }
  void hide(){
    window->iconify ();
  }
  int run(int argc, char *argv[])
  {
    window->show_all();
    return app->run(*window);
  }
  void displayList(vector<ListItemData> & items){
    vector<ListItemData> * data = new vector<ListItemData>(items);
    this->listItemBuffer.setData(data);
    dispatcher.emit();
  }

protected:
  Buffer<vector<ListItemData>*,100> listItemBuffer;
  Window *window;
  Glib::Dispatcher dispatcher;
  Entry *entry;
  Box *box;
  Glib::RefPtr<Gtk::Application> app;
  ListView *list;
  bool onBlur(GdkEventFocus* gdk_event){
    hide();
    return true;
  }
  void onReadyToDisplay(){
    vector<ListItemData> * data=NULL ;
    this->listItemBuffer.getDataNoBlock(&data);
    if(!data)
      return;
    while(this->listItemBuffer.hasMoreData()){
      delete data;
      data=NULL;
      this->listItemBuffer.getDataNoBlock(&data);
      if(!data){
        return;
      }
    }
    this->list->show(data);
    delete data;
  }
  void onTextChanged()
  {
#ifdef DEBUG
    cout << "@CopilotGUI::onTextChanged:" << entry->get_text() << endl;
#endif
#ifndef DEBUG
    TextChangeEvent event(string(entry->get_text()));
    EventCallbackInfo *callback = eventMap.at(EventName::TextChange);
    callback->Call(&event);
#endif
  }
  bool onKeyRelease(GdkEventKey *evt)
  {
#ifdef DEBUG
    cout << "@onKeyRelease:" << evt->string << endl;
#endif
#ifndef DEBUG
    EventCallbackInfo *callback = eventMap.at(EventName::KeyRelease);
    KeyEvent event(
        EventName::KeyRelease,
        evt->state & GDK_SHIFT_MASK,
        evt->state & GDK_CONTROL_MASK,
        evt->state & GDK_MOD1_MASK,
        (int)evt->keyval,
        std::string(evt->string));
    callback->Call(&event);
#endif
    return true;
  }
};
CopilotGUI *gui;

void RegsterEventHandler(EventName name, EventCallbackInfo callback)
{
  eventMap[name] = new EventCallbackInfo(callback);
}

void DisplayList(vector<ListItemData> & items){
  gui->displayList(items);
}
void Show(){
  gui->show();
}
void Hide(){
  gui->hide();
}

int EntryPoint()
{
  gui = new CopilotGUI();
  return gui->run(0, NULL);
}
int main(int argc, char *argv[])
{
  return EntryPoint();
}