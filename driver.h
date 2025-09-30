#include "http.h"
#include <iostream>
class Driver {
public:
  static HTTPResponse get(std::string);
  static HTTPResponse post(std::string);
  static HTTPResponse head(std::string);
};
