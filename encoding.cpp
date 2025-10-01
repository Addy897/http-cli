#include "encoding.h"
#include "logger.h"
#include <vector>
std::string decompressGzip(const std::string& compressed_data) {
    std::string decompressed_data;

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) {
      LOGGER::log_error("decompressGzip()", "inflateInit2 failed.");
        return {};
    }
    const unsigned char* const_ptr = reinterpret_cast<const unsigned char*>(compressed_data.c_str());

    strm.next_in = const_cast<unsigned char*>(const_ptr);
    strm.avail_in = compressed_data.size();

    const size_t CHUNK_SIZE = 16384; // 16KB buffer
    std::vector<unsigned char> buffer(CHUNK_SIZE);
    int ret;

    do {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = buffer.data();
        ret = inflate(&strm, Z_NO_FLUSH);

        switch (ret) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&strm);
                LOGGER::log_error("decompressGzip()", "Decompression error: %s" ,strm.msg);
                return {};
        }

        size_t have = CHUNK_SIZE - strm.avail_out;
        decompressed_data.insert(decompressed_data.end(), buffer.begin(), buffer.begin() + have);

    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);

    return decompressed_data;
}
