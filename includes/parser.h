#pragma once
#include "types.h"
#include <iostream>
#include <map>
class Parser {
public:
  static HEADERS parse_headers(std::string headers);
  static std::string get_line(std::string &buff);
  static std::string parse_html(std::string &buff);
};
