#include "FileProcessing.h"

FileProcessing::FileProcessing(std::string directory_name)
{
	this->directory_name = directory_name + "\\";
	reversed_file = "reversed_file.txt";
	std::ofstream outFile;
	outFile.open(this->directory_name + reversed_file, std::ios::app);
	outFile.close();
	hash_table = new std::unordered_map<std::string, bool*>();
	hash_table->insert(std::make_pair(reversed_file, new bool(true)));
	InitHashMap();
}

FileProcessing::~FileProcessing() 
{
	for (auto it = hash_table->begin(); it != hash_table->end(); ++it)
		delete it->second;
	delete hash_table;
}

std::string FileProcessing::Select(std::string file_name)
{
	std::ifstream inputFile;
	inputFile.open(directory_name + file_name);
	if (inputFile.fail())
	{
		return "Error (Select). File does not exist";
	}
	while (!FileIsAvailable(file_name))
	{
		std::unique_lock<std::mutex> u_lock(mtx);
		condit_var.wait(u_lock);
	}
	{
		std::unique_lock<std::mutex> locker(mtx);
		bool* value = hash_table->find(file_name)->second;
		*value = false;
	}
	std::string str = "";
	std::string line = "";
	while (std::getline(inputFile, line))
	{
		if (line != "")
			str.append(line + "\r\n");
	}
	inputFile.close();
	{
		std::unique_lock<std::mutex> locker(mtx);
		bool* value = hash_table->find(file_name)->second;
		*value = true;
		condit_var.notify_all();
	}
	return str;
}

std::string FileProcessing::Insert(std::string file_name, std::string value, bool include_reversed_file)
{
	if (!include_reversed_file && IsReversedFile(file_name))
		return "Error (Insert). You can't insert to reversed file";
	if (!std::ifstream(directory_name + file_name))
	{
		return "Error (Insert). File does not exist";
	}
	std::ofstream outputFile;
	while (!FileIsAvailable(file_name))
	{
		std::unique_lock<std::mutex> u_lock(mtx);
		condit_var.wait(u_lock);
	}
	{
		std::unique_lock<std::mutex> locker(mtx);
		bool* value = hash_table->find(file_name)->second;
		*value = false;
	}
	outputFile.open(directory_name + file_name, std::ios::app);
	if (outputFile.fail())
	{
		return "Error (Insert). Problems with opening a file";
	}
	value.append("\n");
	outputFile << value;
	outputFile.close();
	{
		std::unique_lock<std::mutex> locker(mtx);
		bool* value = hash_table->find(file_name)->second;
		*value = true;
		condit_var.notify_all();
	}
	return "OK (Insert)";
}

std::string FileProcessing::Update(std::string file_name, std::string value, std::string new_value, bool include_reversed_file)
{
	if (!include_reversed_file && IsReversedFile(file_name))
		return "Error (Update). You can't update reversed file";
	std::ifstream inputFile;
	inputFile.open(directory_name + file_name);
	if (inputFile.fail())
	{
		return "Error (Update). File does not exist";
	}
	while (!FileIsAvailable(file_name))
	{
		std::unique_lock<std::mutex> u_lock(mtx);
		condit_var.wait(u_lock);
	}
	{
		std::unique_lock<std::mutex> locker(mtx);
		bool* value = hash_table->find(file_name)->second;
		*value = false;
	}
	std::string str = "";
	std::string line = "";
	while (std::getline(inputFile, line))
	{
		if (line == value)
		{
			str.append(new_value);
			if (new_value != "")
				str.append("\n");
		}
		else
		{
			str.append(line);
			if (line != "")
				str.append("\n");
		}
	}
	inputFile.close();
	std::ofstream outputFile;
	outputFile.open(directory_name + file_name, std::ios::trunc);
	outputFile << str;
	outputFile.close();
	{
		std::unique_lock<std::mutex> locker(mtx);
		bool* value = hash_table->find(file_name)->second;
		*value = true;
		condit_var.notify_all();
	}
	return "OK (Update)";
}

std::string FileProcessing::Delete(std::string file_name, std::string value)
{
	if (IsReversedFile(file_name))
		return "Error (Delete). You can't delete reversed file";
	std::string result = Update(file_name, value, "");
	if (result.find("Error") == 0)
		result = "Error (Delete). File does not exist";
	else
		result = "OK (Delete)";
	return result;
}

std::string FileProcessing::Truncate(std::string file_name)
{
	if (IsReversedFile(file_name))
		return "Error (Truncate). You can't truncate reversed file";
	std::ifstream inputFile;
	inputFile.open(directory_name + file_name);
	if (inputFile.fail())
	{
		return "Error (Truncate). File does not exist";
	}
	std::ofstream outputFile;
	while (!FileIsAvailable(file_name))
	{
		std::unique_lock<std::mutex> u_lock(mtx);
		condit_var.wait(u_lock);
	}
	{
		std::unique_lock<std::mutex> locker(mtx);
		bool* value = hash_table->find(file_name)->second;
		*value = false;
	}
	outputFile.open(directory_name + file_name, std::ios::trunc);
	std::string result;
	if (outputFile.fail())
		result = "Error (Truncate). Problems with opening a file";
	else
	{
		outputFile.close();
		result = "OK (Truncate)";
	}
	{
		std::unique_lock<std::mutex> locker(mtx);
		bool* value = hash_table->find(file_name)->second;
		*value = true;
		condit_var.notify_all();
	}
	return result;
}

std::string FileProcessing::Drop(std::string file_name)
{
	if (IsReversedFile(file_name))
		return "Error (Drop). You can't drop reversed file";
	Update(reversed_file, file_name, "", true);
	int value = std::remove((directory_name + file_name).c_str());
	std::string result;
	if (value != 0)
		result = "Error (Drop). It is not possible to delete the file";
	else 
	{
		result = "OK (Drop)";
		delete hash_table->find(file_name)->second;
		hash_table->erase(file_name);
	}
	return result;
}

std::string FileProcessing::Create(std::string file_name)
{
	if(IsReversedFile(file_name))
		return "Error (Create). You can't create reversed file";
	if (std::ifstream(directory_name + file_name))
	{
		return "Error (Create). File already exists";
	}
	hash_table->insert(std::make_pair(file_name, new bool(true)));
	std::ofstream outputFile;
	outputFile.open(directory_name + file_name);
	outputFile.close();
	Insert(reversed_file, file_name, true);
	return "OK (Create)";
}

std::string FileProcessing::List()
{
	std::string result = Select(reversed_file);
	return result;
}

bool FileProcessing::IsReversedFile(std::string file_name)
{
	return strcmp(file_name.c_str(), reversed_file.c_str()) == 0;
}

bool FileProcessing::FileIsAvailable(std::string file_name)
{
	std::unique_lock<std::mutex> locker(mtx);
	bool* is_available = hash_table->find(file_name)->second;
	return *is_available;
}

void FileProcessing::InitHashMap()
{
	std::string result = List();
	std::stringstream str_stream(result);
	std::string token;
	while (std::getline(str_stream, token, '\n'))
	{
		if (token != "")
		{
			hash_table->insert(std::make_pair(token.erase(token.find("\r")), new bool(true)));
		}
	}
}

void FileProcessing::Log(std::string input, std::string ip)
{
	char str_time[100];
	memset(str_time, '\0', sizeof(str_time));
	time_t rawtime;
	struct tm * time_info;
	time(&rawtime);
	time_info = localtime(&rawtime);
	strftime(str_time, sizeof(str_time), "  %c  ", time_info);
	{
		std::unique_lock<std::mutex> locker(log_mtx);
		std::ofstream outputFile;
		outputFile.open("log.txt", std::ios::out | std::ios::app);
		std::string value = ip;
		value.append(str_time).append(input).append("\n");

		outputFile << value;
		outputFile.close();
	}
}