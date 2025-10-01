#include "parser.h"
#include "types.h"
#include "utils.h"
using std::string;
string Parser::get_line(string &buff) {
  size_t line_index = buff.find("\r\n");

  if (line_index != string::npos) {
    string line = buff.substr(0, line_index);
    buff = buff.substr(line_index + 1, buff.size());
    return line;
  } else {
    line_index = buff.find("\n");
    if (line_index != string::npos) {
      string line = buff.substr(0, line_index);
      buff = buff.substr(line_index + 1, buff.size());
      return line;
    }
  }
  return buff;
}
HEADERS Parser::parse_headers(std::string raw_header) {
  HEADERS response_headers;
  string line = get_line(raw_header);
  while (!line.empty()) {
    size_t pos = line.find(":");
    if (pos == string ::npos)
      break;
    string key = line.substr(0, pos);
    if (!key.empty()) {
      trim(key);
      lower(key);
      string val = line.substr(pos + 1);
      trim(val);
      response_headers[key] = val;
    }
    line = get_line(raw_header);
  }
  return response_headers;
}
string Parser::parse_html(string &content) {
  string text;
  bool in_tag = false;
  bool in_entity = false;
  bool is_tag_name = false;
  string entity;
  string tag_name;
  for (char &c : content) {
    if (c == '<') {
      in_tag = true;
      tag_name.clear();
      is_tag_name = true;
    } else if (c == '>') {
      in_tag = false;
    } else if (c == '&')
      in_entity = true;
    else if (c == ';' && in_entity) {
      in_entity = false;
      if (entity == "lt")
        text.push_back('<');
      else if (entity == "gt")
        text.push_back('>');
      else if (entity == "amp")
        text.push_back('&');
      entity.clear();
    } else if (!in_tag && tag_name != "style") {
      if (in_entity) {
        if (isalnum(c)) {
          entity.push_back(c);
          continue;
        } else {
          in_entity = false;
          text.append("&" + entity);
          entity.clear();
        }
      }
      text.push_back(c);
    } else {
      if (c == ' ')
        is_tag_name = false;
      if (is_tag_name)
        tag_name.push_back(c);
    }
  }
  return text;
}
