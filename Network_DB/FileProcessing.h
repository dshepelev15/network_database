#pragma once
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <mutex>
#include <windows.h>
#include <time.h>
#include <condition_variable>

class FileProcessing
{
	std::string reversed_file;
	std::string directory_name;
	std::mutex mtx, log_mtx;
	std::condition_variable condit_var;
	std::unordered_map<std::string, bool*>* hash_table;
	
	bool IsReversedFile(std::string file_name);
	bool FileIsAvailable(std::string file_name);
	void InitHashMap();
	void SetFileAvailable(std::string file_name, bool value);
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

