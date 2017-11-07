#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <string>

class Client
{
	const char* ip;
	void Error(std::string msg);
public:
	Client(const char* server_ip);
	~Client();
	int Connect();
};

