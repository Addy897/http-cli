#include "driver.h"
#include "file_reader.h"
#include "http.h"
std::string Driver::get(std::string _url) {
  URL url(_url);
  if (url.scheme() == "file") {
    FileReader f(url.path());
    return f.read_all();
    ;
  }
  HTTPRequest req(url);
  HTTPResponse resp = req.get();
  return resp.body();
}
