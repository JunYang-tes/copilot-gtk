#include "TagCreator.h"
#include "debug.h"
#include <sstream>
using std::ostringstream;
TagWrapper TagCreator::create(Parser *parser)
{
  if (parser->token.type == TokenType::String)
  {
    string name(parser->token.start, parser->token.length);
    parser->match(TokenType::String);
    vector<Token> tokens;
    while (true)
    {
      switch (parser->token.type)
      {
      case TokenType::Comma:
        parser->match(TokenType::Comma);
        break;
      case TokenType::Number:
      case TokenType::String:
        tokens.push_back(parser->token);
        parser->match(parser->token.type);
        break;
      }
      if (parser->token.type == TokenType::RightBracket ||
          parser->token.EOC)
      {
        break;
      }
      else
      {
        //error
      }
    }
    return this->create(name, tokens);
  }
}
// class Reset : public TagCreator
// {
// protected:
//   TagWrapper create(string name, vector<Token> &tokens)
//   {
//     TagWrapper tagWrapper;
//     tagWrapper.name = string(tokens[0].start, tokens[0].length);
//     tagWrapper.clean = true;
//     return tagWrapper;
//   }
// };
TagWrapper Reset::create(string name, vector<Token> &tokens)
{
  TagWrapper tagWrapper;
  tagWrapper.name = string(tokens[0].start, tokens[0].length);
  tagWrapper.clean = true;
  return tagWrapper;
}
// class Color : public TagCreator
// {
// protected:
//   virtual string getTagName() = 0;
//   virtual void setColor(Glib::RefPtr<TextTag> &tag, string color, bool isRgb) = 0;
TagWrapper Color::create(string name, vector<Token> &tokens)
{
  LOG("@Color::create")
  TagWrapper tagWrapper;
  tagWrapper.name = name;
  tagWrapper.clean = false;
  tagWrapper.hasTag = true;
  if (tokens.empty())
  {
    //error handle
  }
  Token first = tokens[0];
  if (first.type == TokenType::Number)
  {
    if (tokens.size() != 3)
    {
      //bad arguments;
    }
    ostringstream color;
    color << "rgba(";
    for (auto it = tokens.begin(); it != tokens.end(); ++it)
    {

      // color << string(it->start, it->.length) << ",";
    }
    color << "1)";

    tagWrapper.tag = TextTag::create(name);
    setColor(tagWrapper.tag, color.str(), true);
    // tagWrapper.tag.property_background_rgba().set_value(color.str().c_str());
    tagWrapper.clean = false;
    return tagWrapper;
  }
  else if (first.type == TokenType::String)
  {
    tagWrapper.tag = TextTag::create(name);
    setColor(tagWrapper.tag, string(first.start, first.length), false);
    // tagWrapper.tag.property_background()
    //     .set_value(string(first.start, first.length).c_str());
    return tagWrapper;
  }
  //error handle
}
