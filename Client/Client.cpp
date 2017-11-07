#include "Client.h"
#include <iostream>

Client::Client(const char* server_ip)
{
	ip = server_ip;
}


Client::~Client()
{
}

int Client::Connect()
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
	{
		Error("Error WSAStartup");
		return 1;
	}
	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr(ip); 
	addr.sin_port = htons(777);
	addr.sin_family = AF_INET;

	SOCKET connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connection == INVALID_SOCKET)
	{
		Error("Socket failed with error");
		return 1;
	}
	if (connect(connection, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(connection);
		Error("Connection error");
		return 1;
	}
	char buffer[512];
	memset(buffer, '\0', sizeof(buffer));
	recv(connection, buffer, sizeof(buffer), NULL);
	std::cout << buffer << std::endl;
	
	if (strcmp(buffer, "Sorry, but server is full") == 0)
	{
		WSACleanup();
		return 1;
	}	
	std::string message;
	while (true)
	{
		std::cout << "Enter your command: ";
		std::getline(std::cin, message);
		message.append("\r");
		send(connection, message.c_str(), message.length(), NULL);
		memset(buffer, '\0', sizeof(buffer));
		if (recv(connection, buffer, sizeof(buffer) - 1, NULL) == -1)
		{
			std::string message = "You were disconnected because of timeout or server was turned off";
			std::cout << message << std::endl;
			break;
		}
		std::cout << buffer << std::endl;
		if (strcmp(buffer, "Result:\r\nBuy buy ...\r\n") == 0)
			break;
	}
	closesocket(connection);
	WSACleanup();
	return 0;
}

void Client::Error(std::string msg)
{
	std::cout << msg << " " << WSAGetLastError() << std::endl;
	WSACleanup();
}
