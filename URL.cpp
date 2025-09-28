#include "URL.h"

URL::URL(string url) {
  this->_url = url;

  parse_url();
}
void URL::parse_url() {
  int index = _url.find("://");
  if (index == string::npos) {
    return;
  }
  _scheme = _url.substr(0, index);
  if (_scheme == "https")
    _port = 443;
  if (_scheme == "file") {
    _path = _url.substr(index + 4, _url.size());
    return;
  }
  string temp_url = _url.substr(index + 3, _url.size());
  index = temp_url.find("/");
  if (index == string::npos) {
    _hostname = temp_url;
    return;
  }
  _hostname = temp_url.substr(0, index);
  _path = temp_url.substr(index, temp_url.size());
}
