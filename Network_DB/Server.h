#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <ws2tcpip.h>

#include "FileProcessing.h"
#include "CommandExecutor.h"

#define THREAD_NUMBER 3

struct ProcessingThread {
	std::thread* c_thread;
	SOCKET connection;
	bool is_active = false;
	bool need_to_close = false;
};

class Server
{
	CommandExecutor* _command_executor;
	ProcessingThread client_threads[THREAD_NUMBER];
	std::mutex mtx, other_mtx;
	std::condition_variable condit_var;


	std::string CreateSubDirectory(std::string path);
	void Error(std::string msg, SOCKET socket);
	void CollectProcessingThreads();
	void HandleClientConnection(ProcessingThread* proc_thread, std::string ip);
	ProcessingThread* GetFreeProcessingThread();
public:
	Server(std::string path);
	~Server();
	int Run(char* ip);
};

