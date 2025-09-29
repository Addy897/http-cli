#include "logger.h"
#include <cstdarg>
#include <cstdio>
#include <iostream>
void LOGGER::log_debug(std::string TAG, const char *format, ...) {
#ifdef LOG_DEBUG
  va_list args;
  va_start(args, format);
  std::cout << BOLDBLUE << "[DEBUG] " << RESET << TAG << ": ";
  vprintf(format, args);
  std::cout << "\n";

  va_end(args);
#endif
}

void LOGGER::log_info(std::string TAG, const char *format, ...) {
#ifdef LOG_INFO
  va_list args;
  va_start(args, format);
  std::cout << BOLDGREEN << "[INFO] " << RESET << TAG << ": ";
  vprintf(format, args);
  std::cout << "\n";
  va_end(args);
#endif
}
void LOGGER::log_error(std::string TAG, const char *format, ...) {
#ifdef LOG_ERROR
  va_list args;
  va_start(args, format);
  std::cout << BOLDRED << "[ERROR] " << RESET << TAG << ": ";
  vprintf(format, args);
  std::cout << "\n";
  va_end(args);
#endif
}
void LOGGER::log_warning(std::string TAG, const char *format, ...) {

#ifdef ERROR
  va_list args;
  va_start(args, format);
  std::cout << BOLDYELLOW << "[WARN] " << RESET << TAG << ": ";
  vprintf(format, args);
  std::cout << "\n";
  va_end(args);
#endif
}
