#include "http_client.h"
#include "logger.h"
#include "socket_client.h"
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdexcept>

// HTTP CLIENT

HTTPClient::HTTPClient() : SocketClient() {
  client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client == INVALID_SOCKET) {
    throw std::runtime_error("socket init failed.");
  }
}
void HTTPClient::conn(std::string hostname, int port) {

  if (hostname.empty()) {
    throw std::runtime_error("hostname is NULL");
  }

  // setup sockaddr
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(port);
  hostent *host = gethostbyname(hostname.c_str());
  if (host == NULL) {
    char error[100];
    snprintf(error, 100, "invalid hostname: %s", hostname.c_str());
    throw std::runtime_error(error);
  }
  if (host->h_addr_list[0] != 0) {
    client_addr.sin_addr.s_addr = *(u_long *)host->h_addr_list[0];
  } else {
    char error[100];
    snprintf(error, 100, "invalid hostname: %s", hostname.c_str());
    throw std::runtime_error(error);
  }

  // establish connection
  if (connect(client, (SOCKADDR *)&client_addr, sizeof(client_addr))) {

    throw std::runtime_error("connection failed.");
  }
}

int HTTPClient::write(std::string data) {
  return send(client, data.c_str(), data.size(), 0);
}

std::string HTTPClient::read(size_t size) {
  char recv_data[CHUNK_SIZE];
  std::string recv_buf;

  if (size <= CHUNK_SIZE) {
    int recv_size = recv(client, recv_data, size, 0);
    recv_buf.append(recv_data, recv_size);
  } else {

    size_t total_size = 0;
    // read all 'size' bytes
    while (total_size < size) {
      size_t temp_size =
          size - total_size > CHUNK_SIZE ? CHUNK_SIZE : size - total_size;
      int bytes = recv(client, recv_data, temp_size, 0);
      if (bytes > 0) {
        total_size += bytes;
        recv_buf.append(recv_data, bytes);
      } else
        break;
    }
  }
  return recv_buf;
}

void HTTPClient::close() {
  if (client != INVALID_SOCKET) {
    ::closesocket(client);
    client = INVALID_SOCKET;
  }
}
HTTPClient::~HTTPClient() { close(); }

// HTTPS Client

HTTPSClient::HTTPSClient() : HTTPClient() {
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();
  ctx = nullptr;
  ctx = SSL_CTX_new(TLS_method());
  if (ctx == nullptr) {
    throw std::runtime_error("SSL Contex failed.");
  }
}
char *HTTPSClient::ossl_err_as_string(void) {
  BIO *bio = BIO_new(BIO_s_mem());

  ERR_print_errors(bio);
  char *buf = NULL;
  size_t len = BIO_get_mem_data(bio, &buf);
  char *ret = (char *)calloc(1, 1 + len);
  if (ret)
    memcpy(ret, buf, len);
  BIO_free(bio);
  return ret;
}
void HTTPSClient::conn(std::string hostname, int port) {
  HTTPClient::conn(hostname, port);
  ssl_client = SSL_new(ctx);
  if (!ssl_client) {

    LOGGER::log_error("conn()", "Unable to create SSL client: %s",
                      ossl_err_as_string());
    throw std::runtime_error("Unable to create SSL Client.");
  }
  SSL_set_fd(ssl_client, client);
  SSL_set_tlsext_host_name(ssl_client, hostname.c_str());
  if (SSL_connect(ssl_client) != 1) {
    LOGGER::log_error("conn()", "Unable to connect SSL client: %s",
                      ossl_err_as_string());
    throw std::runtime_error("SSL connect error.");
  }
}
int HTTPSClient::write(std::string data) {
  return SSL_write(ssl_client, data.c_str(), data.size());
}
std::string HTTPSClient::read(size_t size) {
  char recv_data[CHUNK_SIZE];
  std::string recv_buf;

  if (size <= CHUNK_SIZE) {
    int recv_size = SSL_read(ssl_client, recv_data, size);
    recv_buf.append(recv_data, recv_size);
  } else {

    size_t total_size = 0;
    // read all 'size' bytes
    while (total_size < size) {
      size_t temp_size =
          size - total_size > CHUNK_SIZE ? CHUNK_SIZE : size - total_size;
      int bytes = SSL_read(ssl_client, recv_data, temp_size);
      if (bytes > 0) {
        total_size += bytes;
        recv_buf.append(recv_data, bytes);
      } else
        break;
    }
  }
  return recv_buf;
}
void HTTPSClient::close() {
  if (ssl_client) {
    SSL_shutdown(ssl_client);
    SSL_free(ssl_client);
  }
}

HTTPSClient::~HTTPSClient() {
  close();

  if (ctx != nullptr)
    SSL_CTX_free(ctx);
}
