#pragma once
#include <iostream>
#include <winsock2.h>

#define CHUNK_SIZE 1024
class SocketClient {
protected:
  WSAData wsdata;

  SOCKET client;
  sockaddr_in client_addr;

public:
  SocketClient();
  ~SocketClient();
  virtual void conn(std::string hostname, int port) = 0;
  virtual int write(std::string) = 0;
  virtual std::string read(size_t size) = 0;
  virtual void close() = 0;
};
