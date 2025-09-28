
#include "utils.h"
#include <cctype>

void lower(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c) { return tolower(c); });
}
void trim(std::string &s) {
  auto it = std::find_if_not(s.begin(), s.end(),
                             [](unsigned char ch) { return std::isspace(ch); });
  s.erase(s.begin(), it);

  auto rit = std::find_if_not(
      s.rbegin(), s.rend(), [](unsigned char ch) { return std::isspace(ch); });
  s.erase(rit.base(), s.end());
}
