#include "file_reader.h"
#include <fstream>
#include <sstream>

FileReader::FileReader(std::string path) { in = std::ifstream(path); }
std::string FileReader::read_all() {
  std::ostringstream sstr;
  sstr << in.rdbuf();
  return sstr.str();
}
FileReader::~FileReader() { in.close(); }
