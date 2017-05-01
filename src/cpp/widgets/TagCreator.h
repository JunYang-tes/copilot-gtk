#ifndef COPILOT_TAG_CREATOR
#define COPILOT_TAG_CREATOR
#include <gtkmm.h>
#include <vector>
#include "Parser.h"
using std::vector;
using Gtk::TextTag;
class TagCreator
{
public:
  TagWrapper create(Parser *parser);
  virtual TagWrapper create(string name, vector<Token> &tokens) = 0;
};
class Reset : public TagCreator
{
public:
  TagWrapper create(string name, vector<Token> &tokens);
};
class Color : public TagCreator
{
public:
  TagWrapper create(string name, vector<Token> &tokens);

protected:
  virtual string getTagName() = 0;
  virtual void setColor(Glib::RefPtr<TextTag> &tag, string color, bool isRgba) = 0;
};
class BgColor : public Color
{
protected:
  string getTagName()
  {
    return string("bg");
  }
  void setColor(Glib::RefPtr<TextTag> &tag, string color, bool isRgb)
  {
    tag->property_background().set_value(color);
  }
};
class FgColor : public Color
{
protected:
  string getTagName()
  {
    return string("fg");
  }
  void setColor(Glib::RefPtr<TextTag> &tag, string color, bool isRgb)
  {
    tag->property_foreground().set_value(color);
  }
};

#endif