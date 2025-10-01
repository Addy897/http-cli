#include "driver.h"
#include "logger.h"
#include "parser.h"
#include <cstring>
using namespace std;

struct OPTIONS {
  METHOD m = GET;
  bool parse = false;
  bool show_headers = false;
} OPTIONS;

int usage(const char *executable) {
  cout << "Usage: " << executable << " [OPTIONS..] <URL>\n";
  cout << "-x METHOD (GET,POST,HEAD) default is GET \n";
  cout << "-h Shows Usage\n";
  cout << "-p Parse HTML\n";
  cout << "-sh Show Headers\n";
  cout << "-l LOG LEVEL (ALL, DEBUG, INFO, WARN, ERROR) default is none\n";
  return 0;
}
string url;
bool parse_args(int argc, char **argv) {
  if (argc <= 0)
    return 0;
  if (argc < 2)
    return usage(argv[0]);
  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "-x", 2) == 0) {
      i++;
      if (strncmp(argv[i], "GET", 3) == 0)
        OPTIONS.m = GET;
      else if (strncmp(argv[i], "POST", 4) == 0)
        OPTIONS.m = POST;
      else if (strncmp(argv[i], "HEAD", 4) == 0)
        OPTIONS.m = HEAD;
      else
        cout << "Invalid method: " << argv[i] << " using GET\n";

    } else if (strncmp(argv[i], "-sh", 3) == 0) {
      OPTIONS.show_headers = true;
    } else if (strncmp(argv[i], "-p", 2) == 0) {
      OPTIONS.parse = true;
    } else if (strncmp(argv[i], "-h", 2) == 0) {
      return usage(argv[0]);
    } else if (strncmp(argv[i], "-l", 2) == 0) {
      i++;
      if (strncmp(argv[i], "ALL", 3) == 0) {
        LOGGER::LOG_LEVEL = ALL;
      } else if (strncmp(argv[i], "DEBUG", 5) == 0) {
        LOGGER::LOG_LEVEL = DEBUG;
      } else if (strncmp(argv[i], "INFO", 4) == 0) {
        LOGGER::LOG_LEVEL = INFO;
      } else if (strncmp(argv[i], "WARN", 4) == 0) {
        LOGGER::LOG_LEVEL = WARN;
      } else if (strncmp(argv[i], "ERROR", 5) == 0) {
        LOGGER::LOG_LEVEL = ERR;
      } else {
        LOGGER::LOG_LEVEL = NONE;
      }
    } else {
      url = argv[i];
    }
  }
  if (url.empty())
    return usage(argv[0]);
  return 1;
}
int main(int argc, char **argv) {
  if (parse_args(argc, argv)) {

    std::string content;
    HTTPResponse response;
    switch ((int)OPTIONS.m) {
    case GET:
      response = Driver::get(url);
      response = Driver::get(url);
      break;
    case HEAD:
      response = Driver::head(url);
      break;
    case POST:
      response = Driver::post(url);
      break;
    }
    if (OPTIONS.parse && !response.url().view_source()) {
      content = Parser::parse_html(response.body);
    } else {
      content = response.body;
    }
    if (OPTIONS.show_headers) {
      response.print_headers();
    }
    cout << content;
  }

  return 0;
}
