#include "driver.h"
#include "file_reader.h"
#include "http.h"
#include "logger.h"
#include <stdexcept>
HTTPResponse Driver::get(std::string _url) {
  URL url(_url);
  if (url.scheme() == "file") {
    FileReader f(url.path());
    HTTPResponse resp(url);
    resp.body = f.read_all();
    resp.code = 200;
    resp.status = "file";
    return resp;
  }
  HTTPRequest req(url);
  HTTPResponse resp = req.get();
  return resp;
}
HTTPResponse Driver::post(std::string _url) {
  URL url(_url);
  if (url.scheme() != "http" && url.scheme() != "https") {
    LOGGER::log_error("post()", "Invalid scheme for post method: %s",
                      url.scheme().c_str());
    throw std::runtime_error("Invalid scheme");
  }
  HTTPRequest req(url);
  HTTPResponse resp = req.post();
  return resp;
}
HTTPResponse Driver::head(std::string _url) {
  URL url(_url);
  if (url.scheme() != "http" || url.scheme() != "https") {
    LOGGER::log_error("head()", "Invalid scheme for post method: %s",
                      url.scheme().c_str());
    throw std::runtime_error("Invlaid scheme");
  }
  HTTPRequest req(url);
  HTTPResponse resp = req.head();
  return resp;
}
