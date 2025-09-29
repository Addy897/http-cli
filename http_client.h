#pragma once
#include "socket_client.h"
#include <openssl/ssl.h>
class HTTPClient : public SocketClient {
public:
  HTTPClient();
  ~HTTPClient();
  void conn(std::string hostname, int port) override;
  int write(std::string) override;
  std::string read(size_t size) override;
  void close() override;
};

class HTTPSClient : public HTTPClient {
protected:
  SSL_CTX *ctx;
  SSL *ssl_client;

public:
  HTTPSClient();
  ~HTTPSClient();

  void conn(std::string hostname, int port) override;
  int write(std::string) override;
  std::string read(size_t size) override;
  void close() override;
};
