#pragma once
#include <string>
#include <zlib.h>
class Decoder {
private:
  z_stream m_strm;
  bool m_complete_buffer = true;
  bool m_init = false;

public:
  Decoder(bool complete);
  std::string decompress_gzip(const std::string &compressed_data);
  void end_gzip();
  ~Decoder();
};
