// TcpServerClient.cpp
#include "TcpServer.h"
#include <iostream>

// Конструктор клиента
TcpServer::Client::Client(Socket socket, SocketAddr_in address)
  : socket(socket), address(address), _status(SocketStatus::connected) {}

// Деструктор клиента
TcpServer::Client::~Client() {
  if(socket 
#ifdef _WIN32
     != INVALID_SOCKET
#else
     != -1
#endif
  ) {
    disconnect();
  }
}

// Получить хост клиента
uint32_t TcpServer::Client::getHost() const {
  return address.sin_addr.s_addr;
}

// Получить порт клиента
uint16_t TcpServer::Client::getPort() const {
  return ntohs(address.sin_port);
}

// Отключить клиента
TcpClientBase::status TcpServer::Client::disconnect() {
  if(_status == SocketStatus::disconnected)
    return _status;

  _status = SocketStatus::disconnected;

  // Отключение сокета
  shutdown(socket, SD_BOTH);
#ifdef _WIN32
  closesocket(socket);
#else
  close(socket);
#endif
  socket =
#ifdef _WIN32
      INVALID_SOCKET;
#else
      -1;
#endif
  return _status;
}

// Получить данные от клиента
DataBuffer TcpServer::Client::loadData() {
  if(_status != SocketStatus::connected)
    return DataBuffer();

  DataBuffer buffer;

  // Получение размера сообщения
  uint32_t net_size;
  int bytes_received = recv_all(socket, reinterpret_cast<char*>(&net_size), sizeof(net_size));
  if(bytes_received <= 0) {
    disconnect();
    return DataBuffer();
  }

  buffer.size = ntohl(net_size);

  if(buffer.size <= 0 || buffer.size > MAX_MESSAGE_SIZE) {
    disconnect();
    return DataBuffer();
  }

  buffer.data_ptr = malloc(buffer.size);
  if(!buffer.data_ptr) {
    std::cerr << "Не удалось выделить память для данных.\n";
    disconnect();
    return DataBuffer();
  }

  // Получение самого сообщения
  bytes_received = recv_all(socket, reinterpret_cast<char*>(buffer.data_ptr), buffer.size);
  if(bytes_received <= 0) {
    disconnect();
    free(buffer.data_ptr);
    return DataBuffer();
  }

  return buffer;
}

// Отправить данные клиенту
bool TcpServer::Client::sendData(const void* buffer, const size_t size) const {
    // Если сокет закрыт вернуть false
    if(_status != SocketStatus::connected) return false;

    // Отправляем размер сообщения
    uint32_t net_size = htonl(static_cast<uint32_t>(size));
    if(send(socket, reinterpret_cast<const char*>(&net_size), sizeof(net_size), 0) <= 0)
        return false;

    // Отправляем само сообщение
    if(send(socket, reinterpret_cast<const char*>(buffer), size, 0) <= 0)
        return false;

    return true;
}
