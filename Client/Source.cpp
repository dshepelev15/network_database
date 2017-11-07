#include "Client.h"
#include <iostream>
#include <string>
int main()
{
	std::string ip;
	std::cout << "Enter server ip: ";
	std::getline(std::cin, ip);
	Client client(ip.c_str());
	int connect_result = client.Connect();
	system("pause");
	return connect_result;
}