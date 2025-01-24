/*
  一个简单的 TCP 客户端框架
  Author: R6bandito.
  Date: 2025/1/23.
*/

#include "Client.h"

// 初始化 Winsock
void InitializeWinsock() {
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
    std::cerr << "WSAStratup failed with: " << WSAGetLastError() << std::endl;
    exit(EXIT_FAILURE);
  }
}

// 创建套接字
SOCKET CreateSocket() {
  SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == INVALID_SOCKET) {
    std::cerr << "Socket Create Failed with : " << WSAGetLastError() << std::endl;
    WSACleanup();
    exit(EXIT_FAILURE);
  }
#ifdef _DEBUG
  std::cout << "Socket Created Successfully!" << std::endl;
#endif
  return sockfd;
}

// 连接服务器
void ConnectToServer(SOCKET& sockfd,const char* _IP,const int _PORT) {
  sockaddr_in serveraddr;
  memset(&serveraddr , 0 , sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(_PORT);
  serveraddr.sin_addr.S_un.S_addr = inet_addr(_IP);
  socklen_t addr_len = sizeof(serveraddr);

  if (connect(sockfd, (const sockaddr*) &serveraddr, addr_len) == SOCKET_ERROR) {
    std::cerr << "Connect Failed with : " << WSAGetLastError() << std::endl;
    closesocket(sockfd);
    WSACleanup();
    exit(EXIT_FAILURE);
  }
  std::cout << "Connected to Server Successfully!" << std::endl;
}

// 创建消息
std::string Create_Message() {
  std::string message;
  std::cout << "Enter Message: ";
  std::getline(std::cin, message);
  return message;
}

// 发送数据
void SendData(SOCKET& sockfd,const char* data,const int length){
  if (send(sockfd,data,length,0) == SOCKET_ERROR) {
    std::cerr << "Send Failed with : " << WSAGetLastError() << std::endl;
    closesocket(sockfd);
    WSACleanup();
    exit(EXIT_FAILURE);
  }
}

int main(int argc, const char* argv[]) {
  const char* SERVER_IP = argv[1];
  const int SERVER_PORT = std::stoi(argv[2]);

  InitializeWinsock();
  SOCKET sockfd = CreateSocket();
  ConnectToServer(sockfd, SERVER_IP, SERVER_PORT);
  
  while(true) {
    std::string message = Create_Message();
    SendData(sockfd, message.c_str(), message.length());
    if (message == "exit") 
      break;
  }
  closesocket(sockfd);
  WSACleanup();
  return 0;
}