#include "URL.h"
#include "socket_client.h"
#include "types.h"
#include <bits/stdc++.h>

enum METHOD { GET = 0, POST = 1, HEAD = 2 };
class HTTPResponse;

class BaseHTTPRequest {
protected:
  URL _url;
  HEADERS _headers;

public:
  BaseHTTPRequest(URL url) : _url(url) {};
  const std::map<string, string> &headers() const { return _headers; }
  const URL &url() const { return _url; }
};

class HTTPRequest : public BaseHTTPRequest {
protected:
  const int MAX_REDIRECT = 5;

public:
  static std::map<std::string, SocketClient> pool;

public:
  HTTPRequest(URL url) : BaseHTTPRequest(url) {};

  void add_header(string key, string val) { _headers[key] = val; }
  void add_headers(HEADERS h) { _headers.insert(h.begin(), h.end()); }

  HTTPResponse get(int redirect_times = 0);
  HTTPResponse post();
  HTTPResponse head();

protected:
  std::string http_request(METHOD);
};

class HTTPResponse {
private:
  string _body;

public:
  int code;
  string status;
  string version;
  HEADERS headers;
  BaseHTTPRequest request;
  const URL &url() const { return request.url(); }
  const string &body() const { return _body; }

public:
  HTTPResponse(BaseHTTPRequest req) : request(req) {};
  void set_body(string &content);
};
