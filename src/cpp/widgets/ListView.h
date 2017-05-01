#ifndef COPILOT_LIST_VIEW
#define COPILOT_LIST_VIEW

#include <vector>
#include "../data.h"
#include <gtkmm.h>
#include <iostream>
#include "ListItem.h"
using std::vector;
using std::cout;

class ListView :public Gtk::Box {
  public:
  ListView():Box(Gtk::Orientation::ORIENTATION_VERTICAL),items(){}
  ~ListView(){
    for(uint i=0;i<items.size();i++){
      delete items[i];
    }
  }
  void show(vector<ListItemData> *data){
    if(data->size()>items.size()){
      uint cnt=data->size()-items.size();
      for(uint i=0;i<cnt;i++){
        ListItem *item = new ListItem((*data)[i].icon,(*data)[i].title,(*data)[i].text);
        items.push_back(item);
        this->add(*item);
      }
    }
    if(data->size()<items.size()){
      int cnt = items.size()-data->size();
      for(int i=items.size()-1;cnt>0;i--,cnt--){
        items[i]->hide();
      }
    }
    uint cnt=data->size();
    for(uint i=0;i<cnt;i++){
      items[i]->setTitle((*data)[i].title);
      items[i]->setText((*data)[i].text);
      items[i]->setIcon((*data)[i].icon);
      items[i]->show();
    }
  }
  protected:
  vector<ListItem *> items;
};

#endif