#include "cache_store.h"
#include "logger.h"
#include <memory>

void CacheStore::set(std::string key, std::string content, time_t timestamp) {
  m_cache[key] = std::make_unique<TimedContent>(timestamp, content);
}

void CacheStore::remove(const std::string &key) { m_cache.erase(key); }
void CacheStore::clear() { m_cache.clear(); }

std::string CacheStore::get(const std::string &key) {
  auto it = m_cache.find(key);
  if (it != m_cache.end()) {
    time_t current_timestamp;
    time(&current_timestamp);
    LOGGER::log_info("CacheStore::get()", "cache hit");
    if (current_timestamp < it->second->first) {
      return it->second->second;
    } else {

      LOGGER::log_info("CacheStore::get()", "cache expired");
      m_cache.erase(it);
    }
  }

  LOGGER::log_info("CacheStore::get()", "cache miss");
  return "";
}
