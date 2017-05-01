#ifndef COPILOT_DATA_STRUCTURE
#define COPILOT_DATA_STRUCTURE

class ItemData {};
class ListItemData : public ItemData{
  public:
  ListItemData(const string &title,const string &text,const string &icon):
  title(title),text(text),icon(icon)
  {}
  string title;
  string text;
  string icon;
};
#endif