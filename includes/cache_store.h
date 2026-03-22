#include "types.h"
#include <map>
#include <memory>

class CacheStore {
private:
  Cache m_cache;

  CacheStore() {}

public:
  static CacheStore &get_instance() {
    static CacheStore instance;
    return instance;
  }
  CacheStore(const CacheStore &) = delete;

  void set(std::string, std::string, time_t);
  std::string get(const std::string &);
  void remove(const std::string &);
  void clear();
};
