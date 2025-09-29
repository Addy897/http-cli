#include "driver.h"
#include "parser.h"
using namespace std;
int main(int argc, char **argv) {
  if (argc == 2) {
    string body = Driver::get(argv[1]);

    cout << Parser::parse_html(body);
  } else {
    string body = Driver::get("https://www.example.com/");
    body = Driver::get("https://www.example.com/");
    cout << Parser::parse_html(body);
  }
  return 0;
}
