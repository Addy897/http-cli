#include "socket_client.h"
#include <stdexcept>

SocketClient::SocketClient() {
  // default values for socket
  client = INVALID_SOCKET;
  client_addr = {0};

  // init wsa
  WORD version = MAKEWORD(2, 2);
  int ret = WSAStartup(version, &wsdata);
  if (ret) {
    // LOGGER.log_error("SocketClient()","WSA initialization failed.");
    char error[100];
    sprintf_s(error, "WSA initialization failed: %d.", ret);
    throw std::runtime_error(error);
  }
}

SocketClient::~SocketClient() { WSACleanup(); }
