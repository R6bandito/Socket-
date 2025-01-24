/*
  Select_IO.cc
     select I/O多路复用的示例程序
  
  Author: R6bandito.
  Date: 2025/1/24.
*/

#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <vector>

void Initialize_Winsock() {
  WSADATA wsadata;
  if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0) {
    std::cerr << "WSAStartup Failed" << std::endl;
    exit(EXIT_FAILURE);
  }
}

SOCKET Create_Socket() {
  SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == INVALID_SOCKET) {
    std::cerr << "Get Socket Failed with: " << WSAGetLastError() << std::endl;
    WSACleanup();
    exit(EXIT_FAILURE);
  }
  return sockfd;
}

// 绑定并监听套接字
void Bind_Listen_Socket(SOCKET& sockfd, const char* _IP, const int _PORT, const int MAX_CLIENTS) {
  sockaddr_in addr;
  addr.sin_port = htons(_PORT);
  addr.sin_family = AF_INET;
  addr.sin_addr.S_un.S_addr = inet_addr(_IP);

  if (bind(sockfd, (const sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR) {
    std::cerr << "Bind Socket Failed with: " << WSAGetLastError() << std::endl;
    return;
  }

  if (listen(sockfd, MAX_CLIENTS) == SOCKET_ERROR) {
    std::cerr << "Listen Socket Failed with: " << WSAGetLastError() << std::endl;
    return;
  }
  std::cout << "Listening" << std::endl;
}

// 设置I/O非阻塞
bool setNoBlocking(SOCKET& sockfd) {
  u_long mode = 1;  // 1: 非阻塞 0: 阻塞
  if (ioctlsocket(sockfd, FIONBIO, &mode) == SOCKET_ERROR) {
    std::cerr << "non-Blocking The I/O Failed with: " << WSAGetLastError() << std::endl;
    return false;
  }
  std::cout << "Success non-Blocking The I/O" << std::endl;
  return true;
}

// Select I/O多路复用
void Select_IO_Accept(SOCKET& sockfd, std::vector<SOCKET>& clients) {
  SOCKET newfd;
  fd_set readfds;  // 读写文件描述符集合  
  FD_ZERO(&readfds);  // 清空文件描述符集合
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  FD_SET(sockfd, &readfds);  // 将服务端监听套接字加入文件描述符集合
  SOCKET maxfd = sockfd;  // 最大文件描述符
  for(int i = 0 ; i < clients.size() ; ++i) {
    if(clients[i] > 0) {
      FD_SET(clients[i], &readfds);  //将客户端套接字加入文件描述符集合
      if(clients[i] > maxfd) {
        maxfd = clients[i];  // 更新最大文件描述符
      }
    }
  }

  int activity = select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);  // 阻塞等待文件描述符集合中的套接字有数据可读
  if (activity == SOCKET_ERROR) {
    std::cerr << "Select Failed with: " << WSAGetLastError() << std::endl;
    return;
  }

  if (FD_ISSET(sockfd, &readfds)) {
    newfd = accept(sockfd, nullptr, nullptr); // 接受客户端连接(不需要客户端地址信息)
    if (newfd == INVALID_SOCKET) {
      std::cerr << "Accept Failed with: " << WSAGetLastError() << std::endl;
      return;
    }
    std::cout << "New Coennection: " << newfd << std::endl;
    if (!setNoBlocking(newfd))  // 设置新的客户端I/O为非阻塞
      return;
  }
  for(auto &it : clients) {  // 将新的客户端套接字加入客户端套接字数组
    if(it == 0) {
      it = newfd;
    }
  }
  return;
}

// 处理客户端
void Handle_Client(std::vector<SOCKET>& clients) {

}

int main(int argc, const char* argv[]) {
  const int MAX_CLIENTS = 64;
  const char* _IP = argv[1];
  const int _PORT = std::atoi(argv[2]);
  std::vector<SOCKET> clients;  clients.reserve(MAX_CLIENTS);  // 客户端套接字数组
  for(auto &it : clients) {   //初始化文件描述符集合
    it = 0;
  }

  Initialize_Winsock();
  SOCKET sockfd = Create_Socket();
  setNoBlocking(sockfd);
  Bind_Listen_Socket(sockfd, _IP, _PORT, MAX_CLIENTS);
  while(true) {
    Select_IO_Accept(sockfd, clients);
  }
  return 0;
}
