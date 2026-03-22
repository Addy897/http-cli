#include <fstream>
#include <string>

class FileReader {
private:
  std::ifstream in;

public:
  FileReader(std::string path);
  ~FileReader();
  std::string read(int);
  std::string read_all();
};
