#include "Parser.h"
#include "debug.h"
#include "TagCreator.h"
ostream &operator<<(ostream &out, Token &t)
{
  out << "vvvvvvvvvvvvv\n";
  if (t.start)
  {
    string text(t.start, t.length);
    out << "token:" << text << endl;
  }
  out << " type:";
  switch (t.type)
  {
  case TokenType::Comma:
    out << ",";
    break;
  case TokenType::LeftBracket:
    out << "(";
    break;
  case TokenType::RightBracket:
    out << ")";
    break;
  case TokenType::Number:
    out << "#";
    break;
  case TokenType::String:
    out << "str";
    break;
  }
  out << "\n^^^^^^^^^^^^^\n";
  return out;
}
void Scanner::pushBack(Token token)
{
  this->tokens.push_back(token);
}
Token Scanner::next()
{
  if (!this->tokens.empty())
  {
    Token token = this->tokens.back();
    this->tokens.pop_back();
    return token;
  }
  else
  {
    Token token;
    if (this->isEOC())
    {
      token.EOC = true;
      return token;
    }
    int scannerState = Scanner::STATE_START;
    while (scannerState != Scanner::STATE_END && !this->isEOC())
    {
      switch (scannerState)
      {
      case Scanner::STATE_START:
        token.start = this->bufferIdx;
        scannerState = this->inStart(token);
        break;
      case Scanner::STATE_NUM:
        scannerState = this->inNumber(token);
        break;
      case Scanner::STATE_STR:
        scannerState = this->inString(token);
        break;
      case Scanner::STATE_L_ESC:
        scannerState = this->inLESC(token);
        break;
      case Scanner::STATE_R_ESC:
        scannerState = this->inRESC(token);
        break;
      }
    }
    return token;
  }
}
int Scanner::inStart(Token &token)
{
  char ch = *(this->idx);
  this->idx++;
  if (ch == '(')
  {
    return Scanner::STATE_L_ESC;
  }
  if (ch == ')')
  {
    if (this->isEOC())
    {
      token.EOC = true;
      token.type = TokenType::RightBracket;
      return Scanner::STATE_END;
    }
    return Scanner::STATE_R_ESC;
  }
  token.length++;
  this->putChar2Buf(ch);
  if (ch >= '0' && ch <= '9')
  {
    token.type = TokenType::Number;
    return Scanner::STATE_NUM;
  }
  if (ch == ',')
  {
    token.type = TokenType::Comma;
    return Scanner::STATE_END;
  }
  token.type = TokenType::String;
  return Scanner::STATE_STR;
}
int Scanner::inNumber(Token &token)
{
  char ch = *this->idx;
  if (ch >= '0' && ch <= '9')
  {
    this->idx++;
    this->putChar2Buf(ch);
    token.length++;
    return Scanner::STATE_NUM;
  }
  else
  {
    return Scanner::STATE_END;
  }
}
int Scanner::inString(Token &token)
{
  char ch = *this->idx;
  // if (ch == '(')
  // {
  //   //token.length++;
  //   this->idx++;
  //   //this->putChar2Buf(ch);
  //   return Scanner::STATE_L_ESC;
  // }
  // if (ch == ')')
  // {
  //  // token.length++;
  //   this->idx++;
  //   //this->putChar2Buf(ch);
  //   return Scanner::STATE_R_ESC;
  // }

  if (ch == '(' || ch == ')' || ch == ',' || (ch >= '0' && ch <= '9'))
  {
    return Scanner::STATE_END;
  }
  else
  {
    token.length++;
    this->idx++;
    this->putChar2Buf(ch);
    return Scanner::STATE_STR;
  }
}

int Scanner::inLESC(Token &token)
{
  char ch = *this->idx;
  if (ch == '(')
  {
    this->idx++;
    token.length++;
    this->putChar2Buf(ch);
    return Scanner::STATE_STR;
  }
  else
  {
    token.type = TokenType::LeftBracket;
  }
  return Scanner::STATE_END;
}
int Scanner::inRESC(Token &token)
{
  char ch = *this->idx;
  if (ch == ')')
  {
    this->idx++;
    token.length++;
    this->putChar2Buf(ch);
    return Scanner::STATE_STR;
  }
  else
  {
    token.type = TokenType::RightBracket;
  }
  return Scanner::STATE_END;
}
map<string, TagCreator *> Parser::creators;
Parser::Parser(const char *content)
{
  this->content = content;
  this->scanner = new Scanner(content);
  if (Parser::creators.empty())
  {
    Parser::creators["bg"] = new BgColor();
    Parser::creators["fg"] = new FgColor();
    Parser::creators["reset"] = new Reset();
  }
}
void Parser::match(TokenType expect)
{
  if (expect == this->token.type)
  {
    this->token = this->scanner->next();
  }
  else
  {
    throw "Unexpected token";
  }
}

void Parser::addTagCreator(string name, TagCreator *creator)
{
  creators[name] = creator;
}
Parsed Parser::next()
{
  cout << "@Parser::next\n";
  token = scanner->next();
  Parsed ret;
  if (token.EOC)
  {
    ret.EOC = true;
    return ret;
  }
  else
  {
    ret.EOC = false;
    switch (token.type)
    {
    case TokenType::LeftBracket:
      ret.tag = createTag();
      if (token.type == TokenType::String || token.type == TokenType::Number)
      {
        ret.text = token.start;
        ret.length = token.length;
        return ret;
      }
      else
      {
        ret.text = NULL;
        ret.length = 0;
        scanner->pushBack(token);
      }
      break;
    default:
      ret.text = token.start;
      ret.length = token.length;
      return ret;
    }
  }
}
int Parser::id = 0;
TagWrapper Parser::createTag()
{
  LOG("@Parser::createTag");
  this->match(TokenType::LeftBracket); //(
  string name(token.start, token.length);
  this->match(TokenType::String); // name
  vector<Token> tokens;
  if (token.type == TokenType::Comma)
  {
    while (token.type != TokenType::RightBracket && !token.EOC)
    {
      this->match(TokenType::Comma);
      LOG("@Parser::createTag.while")
      LOG(token);
      tokens.push_back(token);
      this->match(token.type);
    }
  }
  LOG("create ")
  LOG(name)
  LOG(creators.size());
  TagCreator *creator = creators[name];
  auto tag = creator->create(name + std::to_string(id++), tokens);
  LOG("created")
  this->match(TokenType::RightBracket);
  tag.hasTag = true;
  return tag;
}