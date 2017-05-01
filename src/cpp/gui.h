#ifndef COPILOT_GUI_H
#define COPILOT_GUI_H
#include "event.h"
#include "data.h"
#include <string>
#include <vector>
using std::vector;
using std::string;
enum DisplayType {
  LIST,
  GRID
};
void Show();
void Hide();
void DisplayList(vector<ListItemData> & items);
void Display(DisplayType type,const vector<ItemData*> &data);
void RegsterEventHandler(copilot::enums::EventName name,EventCallbackInfo callback);
int EntryPoint();
#endif