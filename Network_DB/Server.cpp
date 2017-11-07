#include "Server.h"

Server::Server(std::string path)
{
	std::string directory = CreateSubDirectory(path);
	_command_executor = new CommandExecutor(directory);
}


Server::~Server()
{
	delete _command_executor;
}

std::string Server::CreateSubDirectory(std::string path)
{
	path.erase(path.find_last_of("\\") + 1);
	path.append("database");
	std::string cmd = "if not exist \"";
	cmd.append(path).append("\" mkdir \"").append(path).append("\"");
	system(cmd.c_str());
	return path;
}


int Server::Run(char* ip)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(1, 0), &wsaData) != 0)
	{
		Error("Error WSAStartup", NULL);
		return 1;
	}

	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(777);
	addr.sin_family = AF_INET;

	SOCKET my_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKET serv_listen;
	
	if((serv_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		Error("Can't create socket", NULL);
		return 1;
	}
	if ((::bind(serv_listen, (SOCKADDR*)&addr, sizeof(addr))) == SOCKET_ERROR)
	{
		Error("Bind error", serv_listen);
		return 1;
	}
	if (listen(serv_listen, SOMAXCONN) == SOCKET_ERROR)
	{
		Error("Listen error", serv_listen);
		return 1;
	}
	SOCKADDR_IN client_addr;
	int size = sizeof(client_addr);
	char client_ip[16];
	SOCKET new_connection;
	std::thread thread_collector(&Server::CollectProcessingThreads, this);
	while (true)
	{
		new_connection = accept(serv_listen, (SOCKADDR*)&client_addr, &size);
		DWORD timeout = 30000;
		if (setsockopt(new_connection, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(DWORD)))
		{
			Error("Setsockopt error", serv_listen);
			return 1;
		}
		InetNtop(AF_INET, &(client_addr.sin_addr), client_ip, sizeof(client_ip));
		ProcessingThread* free_proc_thread = GetFreeProcessingThread();
		if (free_proc_thread == NULL)
		{
			std::string error_message = "Sorry, but server is full";
			send(new_connection, error_message.c_str(), error_message.length(), NULL);
			closesocket(new_connection);
			continue;
		}
		std::cout << "Client connected with ip " << client_ip << std::endl;
		std::string message = "Hello, client, your ip ";
		message.append(client_ip).append("\r\n");
		send(new_connection, message.c_str(), message.length(), NULL);
		free_proc_thread->connection = new_connection;
		free_proc_thread->c_thread = new std::thread(&Server::HandleClientConnection, this, free_proc_thread, std::string(client_ip));
	}
	closesocket(serv_listen);
	WSACleanup();
	return 0;
}


void Server::Error(std::string msg, SOCKET socket)
{
	std::cout << msg << " " << WSAGetLastError() << std::endl;
	if (socket != NULL)
		closesocket(socket);
	WSACleanup();
}

ProcessingThread* Server::GetFreeProcessingThread()
{
	ProcessingThread* result = NULL;
	std::lock_guard<std::mutex> locker(mtx);
	for (int i = 0; i < THREAD_NUMBER; i++)
	{
		if (!client_threads[i].is_active && !client_threads[i].need_to_close)
		{
			client_threads[i].is_active = true;
			result = &client_threads[i];
			break;
		}
	}
	return result;
}

void Server::CollectProcessingThreads()
{
	while (true)
	{
		std::unique_lock<std::mutex> uniq_locker(other_mtx);
		condit_var.wait(uniq_locker);
		std::unique_lock<std::mutex> locker(mtx);
		for (int i = 0; i < THREAD_NUMBER; i++)
		{
			if (!client_threads[i].is_active &&
				client_threads[i].need_to_close)
			{
				client_threads[i].c_thread->join();
				delete client_threads[i].c_thread;
				client_threads[i].is_active = false;
				client_threads[i].need_to_close = false;
			}
		}
		locker.unlock();
	}
}


void Server::HandleClientConnection(ProcessingThread* proc_thread, std::string ip)
{
	char buffer[512];
	while (proc_thread->is_active)
	{
		std::string message = "";
		while (message.find("\r") == std::string::npos)
		{
			memset(buffer, '\0', sizeof(buffer));
			int answer = recv(proc_thread->connection, buffer, sizeof(buffer), NULL);
			if (answer == -1) 
			{
				proc_thread->is_active = false;
				proc_thread->need_to_close = true;			
				std::cout << "Timeout client with ip " << ip << std::endl;
				goto THREAD_EXIT;
			}
			message.append(buffer);
		}
		message.erase(message.find_first_of("\r"));
		std::string output = _command_executor->Execute(message, ip);
		output.append("\r\n");
		if (strcmp(output.c_str(), "Buy buy ...\r\n") == 0)
		{
			proc_thread->is_active = false;
			proc_thread->need_to_close = true;
			std::cout << "Client disconnected with ip " << ip << std::endl;
		}
		output = "Result:\r\n" + output;
		send(proc_thread->connection, output.c_str(), output.length(), NULL);
		Sleep(100);
	}
THREAD_EXIT:
	condit_var.notify_one();
	closesocket(proc_thread->connection);
}