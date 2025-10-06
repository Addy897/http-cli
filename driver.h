#include "http.h"
#include "types.h"
#include <iostream>
class Driver {
public:
  static HTTPResponse get(std::string, HEADERS headers = {});
  static HTTPResponse post(std::string, HEADERS headers = {},
                           std::string data = "");
  static HTTPResponse head(std::string, HEADERS headers = {});
};
