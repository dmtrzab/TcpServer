#include <iostream>
#include "TcpServer.h"
#include <locale>

// Функция для преобразования IP и порта клиента в строку
std::string getHostStr(const TcpServer::Client& client) {
  uint32_t ip = client.getHost();
  char ip_str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, reinterpret_cast<const void*>(&ip), ip_str, INET_ADDRSTRLEN);

  uint16_t port = client.getPort();

  return std::string(ip_str) + ":" + std::to_string(port);
}

int main() {
    std::setlocale(LC_ALL, "");

    // Создание экземпляра сервера
    TcpServer server(8080, [](DataBuffer data, TcpServer::Client& client){
        std::cout << "(" << getHostStr(client) << ")[ " << data.size << " bytes ]: " 
                  << static_cast<char*>(data.data_ptr) << '\n';
        client.sendData("Hello, client!", sizeof("Hello, client!"));
    }, KeepAliveConfig{1, 1, 1}); // Keep alive{ожидание:1s, интервал: 1s, кол-во пакетов: 1};

    // Запуск сервера
    if(server.start() == TcpServer::status::up) {
        std::cout << "Server is up!" << std::endl;
        server.joinLoop(); // Присоединение к циклу обработки клиентов
    } else {
        std::cout << "Server start error! Error code:" << int(server.getStatus()) << std::endl;
        return -1;
    }

    return 0;
}
