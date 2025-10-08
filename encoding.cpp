#include "encoding.h"
#include "logger.h"
#include <vector>
#include <zlib.h>

Decoder::Decoder(bool complete) {
  m_strm.zalloc = Z_NULL;
  m_strm.zfree = Z_NULL;
  m_strm.opaque = Z_NULL;
  m_strm.avail_in = 0;
  m_strm.next_in = Z_NULL;
  if (inflateInit2(&m_strm, MAX_WBITS | 32) != Z_OK) {
    LOGGER::log_error("decompress_gzip()", "inflateInit2 failed.");
    return;
  }
  m_complete_buffer = complete;
  m_init = true;
}
std::string Decoder::decompress_gzip(const std::string &compressed_data) {
  std::string decompressed_data;

  const unsigned char *const_ptr =
      reinterpret_cast<const unsigned char *>(compressed_data.c_str());

  m_strm.next_in = const_cast<unsigned char *>(const_ptr);
  m_strm.avail_in = compressed_data.size();
  const size_t CHUNK_SIZE = 16384;
  std::vector<unsigned char> buffer(CHUNK_SIZE);
  int ret;

  do {
    m_strm.avail_out = CHUNK_SIZE;
    m_strm.next_out = buffer.data();
    ret = inflate(&m_strm, Z_NO_FLUSH);

    switch (ret) {
    case Z_ERRNO:
    case Z_STREAM_ERROR:
    case Z_BUF_ERROR:
    case Z_VERSION_ERROR:
    case Z_NEED_DICT:
    case Z_DATA_ERROR:
    case Z_MEM_ERROR:
      LOGGER::log_error("decompress_gzip()", "Decompression error %d: %s", ret,
                        m_strm.msg);
      end_gzip();
      return {};
    }

    size_t have = CHUNK_SIZE - m_strm.avail_out;
    decompressed_data.append(buffer.begin(), buffer.begin() + have);

  } while (m_strm.avail_out == 0 && ret != Z_STREAM_END);
  if (ret == Z_STREAM_END || m_complete_buffer) {
    end_gzip();
  }
  return decompressed_data;
}
void Decoder::end_gzip() {
  if (m_init)
    inflateEnd(&m_strm);
}
Decoder::~Decoder() { end_gzip(); }
