#pragma once
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <mutex>
#include <windows.h>
#include <time.h>

struct SpinLock
{
	unsigned short for_reading = 0;
	bool for_writting = false;
};

class FileProcessing
{
	std::string reversed_file;
	std::string directory_name;
	std::mutex mtx, log_mtx;
	std::unordered_map<std::string, SpinLock*>* hash_table;
	
	bool IsReversedFile(std::string file_name);
	bool FileIsAvailable(std::string file_name, bool for_writting);
	void InitHashMap();
	
public:
	FileProcessing(std::string directory_name);
	~FileProcessing();
	void Log(std::string input, std::string client_ip);
	std::string List();
	std::string Create(std::string file_name);
	std::string Drop(std::string file_name);
	std::string Select(std::string file_name);
	std::string Truncate(std::string file_name);
	std::string Delete(std::string file_name, std::string value);
	std::string Insert(std::string file_name, std::string value, bool include_reversed_file = false);
	std::string Update(std::string file_name, std::string value, std::string new_value, bool include_reversed_file = false);
};

