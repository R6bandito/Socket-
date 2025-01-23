#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string>

void InitializeWinsock();  // 初始化 Winsock

SOCKET CreateSocket();  // 创建套接字

void ConnectToServer(SOCKET& sockfd,const char* _IP,const int _PORT);  // 连接服务器

std::string Create_Message();  // 创建消息

void SendData(SOCKET& sockfd,const char* data,const int length);  // 发送数据

#endif // __CLIENT_H__