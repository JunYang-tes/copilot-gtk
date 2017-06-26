#ifndef COPILOT_LIST
#define COPILOT_LIST

#include <gtkmm.h>
#include <string>
#include <iostream>
using std::cout;
using std::string;
using Gtk::ListBoxRow;
using Gtk::Box;
using Gtk::Label;
using Gtk::Image;
using Gtk::ScrolledWindow;
using Gtk::Allocation;
using Gtk::SizeRequestMode;


#define ICON_SIZE 64
class ListItem : public Box
{
public:
  ListItem(const string &img, const string &title, const string &text) : Box(Gtk::Orientation::ORIENTATION_HORIZONTAL)
  {
    this->row = new Box(Gtk::Orientation::ORIENTATION_VERTICAL);
    this->title = new Label();
    this->title->set_markup(title);
    this->title->set_halign(Gtk::Align::ALIGN_START );
    // this->title->set_justify(Gtk::Justification::JUSTIFY_LEFT);
    this->title->set_ellipsize(Pango::EllipsizeMode::ELLIPSIZE_END);
    this->row->pack_start(*this->title, true, true, 0);
    
    this->text = new Label();
    this->text->set_halign(Gtk::Align::ALIGN_START );
    this->text->set_markup(text);
    this->text->set_ellipsize(Pango::EllipsizeMode::ELLIPSIZE_END);
    this->text->set_lines(2); 
    this->text->set_line_wrap(true);
    this->row->add(*this->text);

    this->image = new Image();
    try{
    auto pixbuf = Gdk::Pixbuf::create_from_file(img, ICON_SIZE, ICON_SIZE);
    this->image->set(pixbuf);
    }catch(...){
      this->image->clear();
    }
    this->image->set_size_request(ICON_SIZE, ICON_SIZE);
    this->row->set_size_request(-1, ICON_SIZE);
    this->add(*image);
    this->image->property_valign().set_value(Gtk::Align::ALIGN_START);
    this->pack_start(*row, true, true, 0);
    this->show_all();
  }
  ~ListItem()
  {
    delete this->row;
    delete this->title;
    delete this->text;
    delete this->image;
  }
  void setTitle(const string &title){
    this->title->set_markup(title);
  }
  void setText(const string &text){
    this->text->set_markup(text);
  }
  void setIcon(const string &icon){
    try{
      cout<<"show icon "<<icon<<std::endl;
      auto pixbuf = Gdk::Pixbuf::create_from_file(icon,ICON_SIZE,ICON_SIZE);
      this->image->set(pixbuf);
    }catch(...){
      this->image->clear();
    }
  }
  // void get_preferred_width_vfunc(int &minimum_width, int &natural_width) const
  // {
  //   minimum_width = natural_width = 600;
  // }
  void get_preferred_height_vfunc(int &minimum_height, int &natural_height) const
  {
    minimum_height = natural_height = 100;
  }
  SizeRequestMode get_request_mode() const
  {
    return Gtk::SizeRequestMode::SIZE_REQUEST_CONSTANT_SIZE;
  }

private:
  Box *row;
  Label *title;
  Label *text;
  Image *image;
};
#endif