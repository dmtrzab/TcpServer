#ifndef GENERAL_H
#define GENERAL_H

#include <cstdint>
#include <cstring>
#include <cinttypes>
#include <cstdlib>
#include <malloc.h> 

#ifdef _WIN32
// Windows-specific includes and definitions
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h> // For close(), read(), write(), and SHUT_RDWR
#include <netinet/in.h> // For sockaddr_in
#include <netinet/tcp.h> // For TCP options
#include <arpa/inet.h> // For inet_addr()
#endif

// IP 127.0.0.1
extern uint32_t LOCALHOST_IP;

// Код состояния сокета
enum class SocketStatus : uint8_t {
  connected = 0,
  err_socket_init = 1,
  err_socket_bind = 2,
  err_socket_connect = 3,
  disconnected = 4
};

// Буффер данных куда у нас будет приниматься данные от другой стороны
struct DataBuffer {
  int size = 0;
  void* data_ptr = nullptr;

  DataBuffer() = default;
  DataBuffer(int size, void* data_ptr) : size(size), data_ptr(data_ptr) {}
  DataBuffer(const DataBuffer& other) : size(other.size), data_ptr(malloc(size)) {memcpy(data_ptr, other.data_ptr, size);}
  DataBuffer(DataBuffer&& other) : size(other.size), data_ptr(other.data_ptr) {other.data_ptr = nullptr;}
  ~DataBuffer() {if(data_ptr) free(data_ptr); data_ptr = nullptr;}

  bool isEmpty() {return !data_ptr || !size;}
  operator bool() {return data_ptr && size;}
};

// Тип сокета
enum class SocketType : uint8_t {
  client_socket = 0,
  server_socket = 1
};

// Базовый класс TCP клиента
class TcpClientBase {
public:
  typedef SocketStatus status;
  virtual ~TcpClientBase() {};
  virtual status disconnect() = 0;
  virtual status getStatus() const = 0;
  virtual bool sendData(const void* buffer, const size_t size) const = 0;
  virtual DataBuffer loadData() = 0;
  virtual uint32_t getHost() const = 0;
  virtual uint16_t getPort() const = 0;
  virtual SocketType getType() const = 0;
};

#endif // GENERAL_H
