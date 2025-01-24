#ifndef __SERVER_H__
#define __SERVER_H__

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>

//老版本的MinGW不支持C++11的线程库，所以需要引入mingw.thread.h和mingw.mutex.h
#include <mingw.thread.h>
#include <mingw.mutex.h>

#define BUFFER_SIZE 1024

std::vector<SOCKET> Connection_Container;

std::mutex Connection_Mutex;

void InitializeWinsock();  // 初始化 Winsock库

//typedef uint_ptr SOCKET;
SOCKET Create_Socket();  // 创建套接字

void Bind_Socket(SOCKET& socketed,
                  const char* _IP,
                    const uint16_t& _PORT);  // 绑定套接字

void Listen_Socket(SOCKET& socketed);  // 监听套接字

void AcceptConnection(SOCKET& socketed);  // 接受连接

void ReceiveData(SOCKET& socketed);  // 接收数据

void CloseConnection(SOCKET& socketed);  // 关闭连接

void CleanUp();  // 清理

#endif // __SERVER_H__