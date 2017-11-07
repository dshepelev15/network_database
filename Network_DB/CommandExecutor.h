#pragma once
#include "string"
#include "vector"
#include <sstream>
#include <algorithm>

#include "FileProcessing.h"

enum Command
{
	CMD_LIST,
	CMD_CREATE,
	CMD_DROP,
	CMD_TRUNCATE,
	CMD_SELECT,
	CMD_INSERT,
	CMD_UPDATE,
	CMD_DELETE,
	CMD_HELP,
	CMD_QUIT,
	None
};

class CommandExecutor
{
	FileProcessing* _file_processing;

	void ParseInput(std::string input, std::vector<std::string>* tokens);
	Command IdentifyCommand(std::vector<std::string>* tokens);
	std::string RunFileProcessing(Command command, std::vector<std::string>* tokens);
public:
	CommandExecutor(std::string directory);
	~CommandExecutor();

	std::string Execute(std::string input, std::string ip);

};

