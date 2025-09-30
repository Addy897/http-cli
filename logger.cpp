#include "logger.h"
#include <cstdarg>
#include <cstdio>
#include <iostream>
LL LOGGER::LOG_LEVEL  = NONE;
void LOGGER::log_debug(std::string TAG, const char *format, ...) {
  if (LOGGER::LOG_LEVEL == DEBUG || LOG_LEVEL == ALL) {
    va_list args;
    va_start(args, format);
    std::cout << BOLDBLUE << "[DEBUG] " << RESET << TAG << ": ";
    vprintf(format, args);
    std::cout << "\n";

    va_end(args);
  }
}

void LOGGER::log_info(std::string TAG, const char *format, ...) {
  
  if (LOGGER::LOG_LEVEL == INFO || LOG_LEVEL == ALL) {
    va_list args;
    va_start(args, format);
    std::cout << BOLDGREEN << "[INFO] " << RESET << TAG << ": ";
    vprintf(format, args);
    std::cout << "\n";
    va_end(args);
  }
}
void LOGGER::log_error(std::string TAG, const char *format, ...) {

  if (LOGGER::LOG_LEVEL == ERR || LOG_LEVEL == ALL) {
    va_list args;
    va_start(args, format);
    std::cout << BOLDRED << "[ERROR] " << RESET << TAG << ": ";
    vprintf(format, args);
    std::cout << "\n";
    va_end(args);
  }
}
void LOGGER::log_warning(std::string TAG, const char *format, ...) {

   if (LOGGER::LOG_LEVEL == WARN || LOG_LEVEL == ALL) { 
    va_list args;
    va_start(args, format);
    std::cout << BOLDYELLOW << "[WARN] " << RESET << TAG << ": ";
    vprintf(format, args);
    std::cout << "\n";
    va_end(args);
  }
}
