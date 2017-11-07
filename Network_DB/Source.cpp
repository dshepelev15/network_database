#include "Server.h"

int main(int argc, char** argv)
{
	Server server(argv[0]);
	char ip[] = "0.0.0.0";
	int result = server.Run(ip);
	system("pause");
	return result;
}