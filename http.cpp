#include "http.h"
#include "http_client.h"
#include "parser.h"
#include "socket_client.h"
#include <iostream>
#include <memory>
#include <stdexcept>
string HTTPRequest::http_request(METHOD method) {
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
  for (auto &val : _headers) {
    request += val.first + ": " + val.second + "\r\n";
  }
  request += "Connection: close\r\n";
  request += "\r\n\r\n";
  return request;
}

HTTPResponse HTTPRequest::get(int redirect_times) {
  if (redirect_times > MAX_REDIRECT) {
    throw std::runtime_error("Too many rediretcs");
  }
  HTTPResponse response = HTTPResponse(*this);
  string request = http_request(GET);
  std::unique_ptr<SocketClient> client;
  if (_url.scheme() == "https") {
    client = std::make_unique<HTTPSClient>();
  } else {
    client = std::make_unique<HTTPClient>();
  }

  client->conn(_url.hostname(), _url.port());

  client->write(request);

  string result;
  string delimeter = "\r\n\r\n";
  char c[CHUNK_SIZE];
  int end_index = -1;
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
  if (response.code == 301 || response.code == 302) {
    if (response.headers.find("location") != response.headers.end()) {
      string location = response.headers["location"];
      std::cout << (location[0] == ' ') << "\n";
      std::cout << location << "\n";
      if (location[0] == '/') {
        _url.setPath("/");
      } else {
        URL new_url(location);
        _url = new_url;
      }
      client->close();
      return get(redirect_times + 1);
    }
  }

  string raw_content =
      result.substr(end_index + delimeter.size(), result.size());

  if (response.headers.find("content-length") != response.headers.end()) {
    int content_length =
        stoi(response.headers["content-length"]) - raw_content.size();
    if (content_length < 0)
      content_length = 0;
    string temp = client->read(content_length);

    raw_content.append(temp);
  }
  response.set_body(raw_content);
  client->close();

  return response;
}
HTTPResponse HTTPRequest::post() {
  HTTPResponse response = HTTPResponse(*this);
  string request = http_request(POST);
  std::unique_ptr<SocketClient> client;
  if (_url.scheme() == "https") {
    client = std::make_unique<HTTPSClient>();
  } else {
    client = std::make_unique<HTTPClient>();
  }

  client->conn(_url.hostname(), _url.port());

  client->write(request);
  string result;
  string delimeter = "\r\n\r\n";
  char c[CHUNK_SIZE];
  int end_index = -1;
  while (true) {
    result.append(client->read(CHUNK_SIZE));
    end_index = result.find(delimeter);
    if (end_index != string::npos)
      break;
  }
  string raw_headers = result.substr(0, end_index + 2);
  string line = Parser::get_line(raw_headers);
  std::stringstream lineparser(line);
  lineparser >> response.version >> response.code;
  std::getline(lineparser, response.status);

  response.headers = Parser::parse_headers(raw_headers);
  string raw_content =
      result.substr(end_index + delimeter.size(), result.size());
  if (response.headers.find("content-length") != response.headers.end()) {
    int content_length =
        stoi(response.headers["content-length"]) - raw_content.size();
    if (content_length < 0)
      content_length = 0;
    string temp = client->read(content_length);
    raw_content.append(temp, content_length);
  }
  response.set_body(raw_content);
  client->close();

  return response;
}
HTTPResponse HTTPRequest::head() {
  HTTPResponse response = HTTPResponse(*this);
  string request = http_request(HEAD);
  std::unique_ptr<SocketClient> client;
  if (_url.scheme() == "https") {
    client = std::make_unique<HTTPSClient>();
  } else {
    client = std::make_unique<HTTPClient>();
  }

  client->conn(_url.hostname(), _url.port());

  client->write(request);
  string result;
  string delimeter = "\r\n\r\n";
  char c[CHUNK_SIZE];
  int end_index = -1;
  while (true) {
    result.append(client->read(CHUNK_SIZE));
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
  client->close();

  return response;
}

void HTTPResponse::set_body(string &content) {
  _body = content;
  return;
}
