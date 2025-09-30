#include <string>
using std::string;
class URL {

private:
  string _url;
  string _path = "/";
  string _hostname;
  int _port = 80;
  string _scheme;
  bool _view_source = false;

public:
  int port() const { return _port; }
  bool view_source() const { return _view_source; }
  const std::string &url() const { return _url; }
  const std::string &path() const { return _path; }
  const std::string &scheme() const { return _scheme; }
  const std::string &hostname() const { return _hostname; }

private:
  void parse_url();

public:
  URL() {}
  URL(string url);
  void setPath(std::string path) { _path = path; }
};
