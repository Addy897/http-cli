#pragma once
#include "URL.h"
#include "socket_client.h"
#include "types.h"
#include <bits/stdc++.h>
#include <json/json.h>
#include <memory>
enum METHOD { GET = 0, POST = 1, HEAD = 2 };
class HTTPResponse;

class BaseHTTPRequest {
protected:
  URL _url;
  HEADERS _headers;

public:
  BaseHTTPRequest() {}
  BaseHTTPRequest(URL url) : _url(url) {};
  const std::map<string, string> &headers() const { return _headers; }
  const URL &url() const { return _url; }
};

class HTTPRequest : public BaseHTTPRequest {
protected:
  const int MAX_REDIRECT = 5;
  std::shared_ptr<SocketClient> get_client();
  std::string m_json;
  bool m_asjson;

public:
  static std::map<std::string, std::shared_ptr<SocketClient>> pool;

public:
  HTTPRequest(URL url) : BaseHTTPRequest(url) {};
  void add_header(string key, string val) { _headers[key] = val; }
  void add_headers(HEADERS h) { _headers.insert(h.begin(), h.end()); }

  HTTPResponse get(int redirect_times = 0);
  HTTPResponse post(std::string json = "", bool asjson = true);
  HTTPResponse head(int redirect_times = 0);

protected:
  std::string build_request(METHOD);
  void read_headers(HTTPResponse &);
  void read_body(HTTPResponse &);
  void send_request(METHOD);
  void handle_chunks(HTTPResponse &);
  void cache_body(HTTPResponse &);
};

class HTTPResponse {

public:
  int code;
  string status;
  string version;
  string body;
  HEADERS headers;
  BaseHTTPRequest request;
  const URL &url() const { return request.url(); }

public:
  HTTPResponse() {}
  HTTPResponse(BaseHTTPRequest req) : request(req) {};

  void print_headers();
};
