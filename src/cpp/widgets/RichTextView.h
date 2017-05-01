#ifndef COPILOT_RICH_TEXT_VIEW
#define COPILOT_RICH_TEXT_VIEW
#include "Parser.h"
#include <gtkmm.h>
#include <map>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
using std::vector;
using std::string;
using std::ostream;
using std::cout;
using std::endl;
using std::map;
using Gtk::TextView;
using Gtk::TextTag;

inline bool startsWith(string s1,string s2){
  if(s1.size()<s2.size()){
    return false;
  }
  for(uint i=0;i<s2.size();i++){
    if(s1[i]!=s2[i]){
      return false;
    }
  }
  return true;
}

class RichTextView : public TextView
{
public:
  RichTextView(const char *string) //: TextView()
  {
    auto textbuffer = Gtk::TextBuffer::create();
    auto tag = textbuffer->create_tag();
    // tag->property_background().set_value("red");
    tag->property_size_points().set_value(24);
    Parser parser(string);
    Parsed parsed = parser.next();
    std::vector<Glib::RefPtr<Gtk::TextTag>> tags;
    // tags.push_back(tag);

    while (!parsed.EOC)
    {
      TagWrapper tagWrapper = parsed.tag;
      if (tagWrapper.hasTag)
      {
        if (tagWrapper.clean)
        {
          //clean
          auto it = tags.end();
          int i=tags.size()-1;
          bool found = false;
          while(it>tags.begin()){
            Glib::RefPtr<TextTag> tag = tags[i--];
            auto tagName = tag->property_name().get_value();
            if(startsWith(tagName,parsed.tag.name)){
              found=true;
              break;
            }
            it--;
          }
          if(found){
            tags.erase(it);
          }
        }
        else
        {
          tags.push_back(tagWrapper.tag);
          textbuffer->get_tag_table()->add(tagWrapper.tag);
        }
      }
      if (parsed.text)
      {
          textbuffer->insert_with_tags(textbuffer->end(),
                                       Glib::ustring(parsed.text, parsed.length), tags);
        cout << std::string(parsed.text, parsed.length) << endl;
      }
      parsed = parser.next();
    }
    this->set_buffer(textbuffer);
  }
};
#endif