#include "http.h"
#include "http_client.h"
#include "logger.h"
#include "parser.h"
#include "socket_client.h"

#include <errno.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <winsock2.h>
std::map<std::string, std::shared_ptr<SocketClient>> HTTPRequest::pool;
string HTTPRequest::build_request(METHOD method) {
  string m;
  switch (method) {
  case GET:
    m = "GET";
    break;
  case POST:
    m = "POST";
    break;
  case HEAD:
    m = "HEAD";
    break;
  }
  string request = m + " " + _url.path() + " HTTP/1.1\r\n";
  request += "Host: " + _url.hostname() + "\r\n";
  for (auto &val : _headers)
    request += val.first + ": " + val.second + "\r\n";
  request += "Connection: keep-alive\r\n";
  request += "\r\n\r\n";
  return request;
}
std::shared_ptr<SocketClient> HTTPRequest::get_client() {

  std::shared_ptr<SocketClient> client;

  std::string s_url = _url.url();
  auto it = pool.find(s_url);

  if (it != pool.end()) {
    client = it->second;
  } else {
    if (_url.scheme() == "https") {
      client = std::make_shared<HTTPSClient>();
    } else if (_url.scheme() == "http") {
      client = std::make_shared<HTTPClient>();
    } else {
      LOGGER::log_error("get_client()", "got unknown scheme: %s",
                        _url.scheme().c_str());
      throw std::runtime_error("unknown scheme");
    }
    client->conn(_url.hostname(), _url.port());
    pool[s_url] = client;
  }
  return client;
}
void HTTPRequest::handle_chunks(HTTPResponse &response) { return; }
void HTTPRequest::send_request(METHOD method) {
  string request = build_request(method);

  std::shared_ptr<SocketClient> client = get_client();

  int write_bytes = client->write(request);
  if (write_bytes == -1) {
    if (EPIPE == WSAGetLastError()) {
      HTTPRequest::pool.erase(_url.url());
      return send_request(method);
    }
  }
}
void HTTPRequest::read_headers(HTTPResponse &response) {
  std::shared_ptr<SocketClient> client = get_client();
  string result;
  string delimeter = "\r\n\r\n";
  size_t end_index;
  while (true) {
    string chunk = client->read(CHUNK_SIZE);
    if (chunk.empty())
      break;
    result.append(chunk);
    end_index = result.find(delimeter);
    if (end_index != string::npos)
      break;
  }
  string raw_headers = result.substr(0, end_index + delimeter.size());
  string line = Parser::get_line(raw_headers);
  std::stringstream lineparser(line);
  lineparser >> response.version >> response.code;
  std::getline(lineparser, response.status);
  response.headers = Parser::parse_headers(raw_headers);
  string partial_body =
      result.substr(end_index + delimeter.size(), result.size());

  response.body.append(partial_body);
}
void HTTPRequest::read_body(HTTPResponse &response) {
  auto client = get_client();
  if (response.headers.count("transfer-encoding")) {
    handle_chunks(response);
  }
  if (response.headers.count("content-length")) {
    int content_length =
        stoi(response.headers["content-length"]) - response.body.size();
    if (content_length < 0)
      content_length = 0;
    string temp = client->read(content_length);

    response.body.append(temp);
  }
}
HTTPResponse HTTPRequest::get(int redirect_times) {
  if (redirect_times > MAX_REDIRECT) {
    throw std::runtime_error("Too many rediretcs");
  }

  HTTPResponse response = HTTPResponse(*this);
  send_request(GET);
  read_headers(response);

  LOGGER::log_debug("get()", "response code: %d", response.code);

  if (response.code == 301 || response.code == 302) {
    if (response.headers.find("location") != response.headers.end()) {
      string location = response.headers["location"];
      if (location[0] == '/') {
        _url.setPath(location);
      } else {
        URL new_url(location);
        _url = new_url;
      }
      return get(redirect_times + 1);
    }
  }
  read_body(response);
  return response;
}
HTTPResponse HTTPRequest::post() {
  HTTPResponse response = HTTPResponse(*this);
  send_request(POST);
  read_headers(response);

  LOGGER::log_debug("post()", "response code: %d", response.code);

  read_body(response);
  return response;
}
HTTPResponse HTTPRequest::head(int redirect_times) {
  HTTPResponse response = HTTPResponse(*this);
  send_request(HEAD);
  read_headers(response);

  if (response.code == 301 || response.code == 302) {
    if (response.headers.find("location") != response.headers.end()) {
      string location = response.headers["location"];
      if (location[0] == '/') {
        _url.setPath(location);
      } else {
        URL new_url(location);
        _url = new_url;
      }
      return head(redirect_times + 1);
    }
  }

  return response;
}
