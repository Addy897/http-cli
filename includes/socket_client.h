#pragma once
#include "types.h"
#include <iostream>

#if _WIN32
#include <winsock2.h>
#elif defined(__linux__)
#include <sys/socket.h>
typedef int SOCKET;
  #include <pthread.h>
  #include <unistd.h>
  #include <arpa/inet.h>
       #include <netdb.h>
  #include <netinet/in.h>
  #include <errno.h>
 #include <sys/ioctl.h>
  typedef void* LPVOID;
  #define closesocket close
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define SOCKADDR        struct sockaddr
  #define SOCKADDR_IN     struct sockaddr_in
  #define WSAEWOULDBLOCK EWOULDBLOCK
#define ioctlsocket ioctl
  inline int WSAGetLastError(){
	return errno;
  }
#endif


#define CHUNK_SIZE 1024
class SocketClient {
protected:
#ifdef _WIN32
  WSAData wsdata;
#endif
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
