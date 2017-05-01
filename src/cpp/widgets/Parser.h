#ifndef COPILOT_PARSER
#define COPILOT_PARSER
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
enum TokenType
{
  String,
  Number,
  Comma,
  LeftBracket,
  RightBracket
};
struct Token
{
  Token()
  {
    this->start = NULL;
    this->length = 0;
    this->type = TokenType::String;
    this->EOC = false;
  }
  char *start;
  int length;
  TokenType type;
  bool EOC;
  friend ostream &operator<<(ostream &out, Token &t);
};
struct TagWrapper
{
  TagWrapper():hasTag(false) {
    
  }
  bool hasTag;
  Glib::RefPtr<TextTag> tag;
  bool clean;
  string name;
};
struct Parsed
{
  //what the style tag is
  TagWrapper tag;
  //where the text started
  char *text;
  //where the text ended;
  int length;
  bool error;
  bool EOC;
};

class Scanner
{
public:
  Scanner(const char *content) : content(content)
  {
    this->length = strlen(content);
    this->idx = this->content;
    this->buffer = new char[this->length];
    this->bufferIdx = this->buffer;
  }
  ~Scanner()
  {
    delete[] this->buffer;
  }
  Token next();
  void pushBack(Token token);
  inline bool isEOC(){
    return this->idx >= this->content + this->length;
  }

private:
  const char *content;
  int length;
  char *buffer;
  char *bufferIdx;
  const char *idx;
  vector<Token> tokens;
  int inStart(Token &token);
  int inNumber(Token &token);
  int inString(Token &token);
  int inLESC(Token &token);
  int inRESC(Token &token);
  inline void putChar2Buf(char ch)
  {
    *(this->bufferIdx) = ch;
    this->bufferIdx++;
  }
  static const int STATE_START = 0;
  static const int STATE_END = 1;
  static const int STATE_NUM = 2;
  static const int STATE_STR = 3;
  static const int STATE_L_ESC = 4;
  static const int STATE_R_ESC = 5;
};
class TagCreator;
class Parser
{
public:
  Parser(const char *content);
  ~Parser(){
    delete this->scanner;
  }
  static void addTagCreator(string name, TagCreator *creator);
  Parsed next();
  friend TagCreator;

protected:
  TagWrapper
  createTag();
  void match(TokenType expect);
  // void match();

private:
  const char *content;
  Scanner *scanner;
  static int id;
  static map<string, TagCreator *> creators;
  Token token;
};
#endif