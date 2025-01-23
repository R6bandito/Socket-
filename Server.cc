/*
  一个使用 Winsock 实现的 TCP 简单服务端框架。
  Author: R6bandito.
  Date: 2025-01-22.
*/

#include "Server.h"

// 初始化 Winsock 库
void InitializeWinsock() {
  WSADATA wsadata;
  if (WSAStartup(MAKEWORD(2,2),&wsadata) != 0) {
    std::cerr << "WSAStartup Failed with : " << WSAGetLastError() << std::endl;
    WSACleanup();
    exit(EXIT_FAILURE);
  }

#ifdef _DEBUG
  std::cout << "WSAStartup Success!" << std::endl;
#endif
}

// 创建套接字
SOCKET Create_Socket() {
  SOCKET socketed = socket(AF_INET,SOCK_STREAM,0);
  if (socketed == INVALID_SOCKET) {
    std::cerr << "Socket Creation Failed with : " << WSAGetLastError() << std::endl;
    WSACleanup();
    exit(EXIT_FAILURE);
  }
#ifdef _DEBUG
  std::cout << "Socket Successfully Created!" << std::endl;
#endif
  return socketed;
}

// 绑定套接字
void Bind_Socket(SOCKET& socketed,const char* _IP,const uint16_t& _PORT) {
  struct sockaddr_in ServerAddr;
  ServerAddr.sin_family = AF_INET;
  ServerAddr.sin_addr.S_un.S_addr = inet_addr(_IP);
  //ServerAddr.sin_addr.S_un.S_addr = htonl(_IP);
  ServerAddr.sin_port = htons(_PORT);

  int result = bind(socketed,(struct sockaddr*) &ServerAddr,sizeof(ServerAddr));
  if (result == SOCKET_ERROR) {
    std::cerr << "Socket Binding Failed with : " << WSAGetLastError() << std::endl;
    WSACleanup();
    exit(EXIT_FAILURE);
  }
#ifdef _DEBUG
  std::cout << "Socket Successfully Bound!" << std::endl;
#endif
} 

// 监听套接字
void Listen_Socket(SOCKET& socketed) {
  int result = listen(socketed,10);
  if (result == SOCKET_ERROR) {
    std::cerr << "Socket Listening Failed with : " << WSAGetLastError() << std::endl;
    WSACleanup();
    exit(EXIT_FAILURE);
  }
#ifdef _DEBUG
  std::cout << "Socket Successfully Listening!" << std::endl;
#endif
}

// 接受连接
void AcceptConnection(SOCKET& socketed) {
  struct sockaddr_in ClientAddr;
  memset(&ClientAddr,0,sizeof(ClientAddr));
  ClientAddr.sin_family = AF_INET;
  int ClientAddr_len = sizeof(ClientAddr);
  std::cout << "Waiting for Connection..." << std::endl;
  SOCKET new_socket = accept(socketed,(struct sockaddr*) &ClientAddr, &ClientAddr_len);
  if (socketed == INVALID_SOCKET) {
    std::cerr << "Socket Accept Failed with : " << WSAGetLastError() << std::endl;
    return;
  }

// 使用互斥锁保护 Connection_Container操作，将新连接的套接字加入到容器中
  {
    std::lock_guard<std::mutex> lock(Connection_Mutex);
    Connection_Container.push_back(new_socket);
    std::thread(ReceiveData,std::ref(new_socket)).detach();
  }
  std::cout << "Connection Accepted!" << std::endl;
}

// 接收数据
void ReceiveData(SOCKET& socketed) {
  char buffer[1024];
  while(true) {
    int result = recv(socketed,buffer,BUFFER_SIZE,0);
    if (result == SOCKET_ERROR) {
      std::cerr << "Socket Receive Failed with : " << WSAGetLastError() << std::endl;
      WSACleanup();
      exit(EXIT_FAILURE);
    }
    if (result == 0) {
      std::cout << "Connection Closed by Client!" << std::endl;
      CloseConnection(socketed);
      break;
    }
    std::cout << "Received Data: " << buffer << std::endl;
  }
// 使用互斥锁保护 Connection_Container操作，将连接的套接字从容器中移除
  {
    std::lock_guard<std::mutex> lock(Connection_Mutex);
    auto it = std::find_if(Connection_Container.begin(), Connection_Container.end(),
        [socketed](SOCKET s) { return s == socketed; });
    if (it != Connection_Container.end()) {
        Connection_Container.erase(it);
    }
    std::cout << "Connection Has Been Removed!" << std::endl;
  }
}

// 关闭连接
void CloseConnection(SOCKET& socketed) {
  int result = closesocket(socketed);
  if (result == SOCKET_ERROR) {
    std::cerr << "Socket Close Failed with : " << WSAGetLastError() << std::endl;
    WSACleanup();
    exit(EXIT_FAILURE);
  }
  std::cout << "Connection Closed!" << std::endl;
}

// 清理
void CleanUp() {
  for(auto& s : Connection_Container) {
    CloseConnection(s);
  }
  WSACleanup();
}

int main(int argc,const char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <IP> <PORT>" << std::endl;
    return EXIT_FAILURE;
  }
  const char* _IP = argv[1];
  const uint16_t _PORT = std::stoi(argv[2]);
  
  InitializeWinsock();
  SOCKET socketed = Create_Socket();
  Bind_Socket(socketed,_IP,_PORT);
  Listen_Socket(socketed);
  while(true) {
    AcceptConnection(socketed);
  }
  CleanUp();
  return 0;
}