#include "http.h"
#include "cache_store.h"
#include "encoding.h"
#include "http_client.h"
#include "logger.h"
#include "parser.h"
#include "socket_client.h"

#include <cstddef>
#include <errno.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <winsock2.h>
std::map<std::string, std::shared_ptr<SocketClient>> HTTPRequest::pool;

HTTPRequest::HTTPRequest(URL _url, string data, bool asjson)
    : BaseHTTPRequest(_url) {
  m_json = data;
  m_asjson = asjson;
}

string HTTPRequest::build_request(METHOD method) {
  string m;
  string body = "";
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
  request += "\r\n";

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
void HTTPRequest::handle_chunks(HTTPResponse &response) {
  auto client = get_client();

  std::string data_buffer = response.body;
  response.body.clear();
  while (true) {
    size_t size_line_end = data_buffer.find("\r\n");

    while (size_line_end == std::string::npos) {
      std::string chunk = client->read(CHUNK_SIZE);
      if (chunk.empty()) {
        LOGGER::log_error("handle_chunks()", "empty chunk.");
        return;
      }
      data_buffer.append(chunk);
      size_line_end = data_buffer.find("\r\n");
    }

    std::string size_line = data_buffer.substr(0, size_line_end);
    data_buffer.erase(0, size_line_end + 2);
    size_t chunk_len;
    try {
      chunk_len = std::stoul(size_line, nullptr, 16);
    } catch (const std::exception &e) {
      LOGGER::log_error("handle_chunks()",
                        "Failed to parse chunk size: %s. Error: %s",
                        size_line.c_str(), e.what());
      return;
    }

    if (chunk_len == 0) {
      break;
    }

    LOGGER::log_debug("handle_chunks()",
                      "chunk_len: 0x%s(%d) current_buffer: %d",
                      size_line.c_str(), chunk_len, data_buffer.size());
    size_t remaining_len = chunk_len;

    size_t available_in_buffer = std::min(remaining_len, data_buffer.size());
    response.body.append(data_buffer.substr(0, available_in_buffer));
    data_buffer.erase(0, available_in_buffer);
    remaining_len -= available_in_buffer;
    if (remaining_len > 0) {
      std::string chunk_data = client->read(remaining_len);
      while (chunk_data.size() != remaining_len) {
        LOGGER::log_warning("handle_chunks()",
                            "invalid size of chunks %d, expected %d (retrying)",
                            chunk_data.size(), remaining_len);
        std::string temp = client->read(remaining_len - chunk_data.size());

        if (temp.empty()) {
          break;
        }
        chunk_data += temp;
      }
      response.body.append(chunk_data);
    }
    if (data_buffer.empty())
      client->read(2);
    else
      data_buffer.erase(0, 2);
  }
}
void HTTPRequest::send_request(METHOD method) {
  if (method == POST) {
    if (!m_json.empty()) {
      if (m_asjson) {
        add_header("Content-Type", "application/json");
        add_header("Content-Length", std::to_string(m_json.size()));
      }
    }
  }
  string request = build_request(method);

  LOGGER::log_debug("send_request()", "REQUEST: \n%s\n", request.c_str());
  if (method == POST && !m_json.empty()) {
    request += m_json;
  }

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

  response.body = partial_body;
}
void HTTPRequest::read_body(HTTPResponse &response) {
  auto client = get_client();
  if (response.headers.count("transfer-encoding")) {
    size_t chunked_index =
        response.headers["transfer-encoding"].find("chunked");

    if (chunked_index != string::npos)
      handle_chunks(response);
  } else if (response.headers.count("content-length")) {
    int content_length =
        stoi(response.headers["content-length"]) - response.body.size();
    if (content_length > 0) {
      string temp = client->read(content_length);

      response.body.append(temp);
    }
  }
  if (response.headers.count("content-encoding") &&
      response.headers["content-encoding"] == "gzip") {
    response.body = decompressGzip(response.body);
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
  CacheStore &store = CacheStore::get_instance();
  std::string content = store.get(_url.url());
  if (!content.empty())
    response.body = content;
  else {
    read_body(response);
    cache_body(response);
  }
  return response;
}
void HTTPRequest::cache_body(HTTPResponse &response) {

  CacheStore &store = CacheStore::get_instance();
  auto it = response.headers.find("cache-control");
  if (it != response.headers.end()) {
    string delimeter = "max-age=";
    size_t delimeter_index = it->second.find(delimeter);
    if (delimeter_index == string::npos) {
      delimeter = "no-store";
      delimeter_index = it->second.find(delimeter);
      if (delimeter_index != string::npos) {
        return;
      }
    } else {
      time_t timestamp =
          std::stoul(it->second.substr(delimeter_index + delimeter.size()));
      timestamp += time(NULL);
      store.set(_url.url(), response.body, timestamp);
      return;
    }
  }
  time_t timestamp = time(NULL);
  struct tm datetime = *localtime(&timestamp);
  datetime.tm_sec += 1;
  store.set(_url.url(), response.body, mktime(&datetime));
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

void HTTPResponse::print_headers() {
  for (const auto &it : headers) {

    std::cout << it.first << ": " << it.second << "\n";
  }
}
