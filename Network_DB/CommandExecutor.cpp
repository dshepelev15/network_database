#include "CommandExecutor.h"


CommandExecutor::CommandExecutor(std::string directory)
{
	_file_processing = new FileProcessing(directory);
}


CommandExecutor::~CommandExecutor()
{
}


std::string CommandExecutor::Execute(std::string input, std::string ip)
{
	std::vector<std::string>* tokens = new std::vector<std::string>();
	ParseInput(input, tokens);
	if (tokens->size() == 0)
	{
		delete tokens;
		return "Unsuitable command. Use 'help' for more information";
	}
	_file_processing->Log(input, ip);
	Command command = IdentifyCommand(tokens);
	
	std::string result = RunFileProcessing(command, tokens);
	delete tokens;
	return result;
}

void CommandExecutor::ParseInput(std::string input, std::vector<std::string>* tokens)
{
	std::stringstream str_stream(input);
	std::string token;
	while (std::getline(str_stream, token, ' '))
	{
		if (token != "")
		{
			std::transform(token.begin(), token.end(), token.begin(), tolower);
			tokens->push_back(token);
		}
	}
}

Command CommandExecutor::IdentifyCommand(std::vector<std::string>* tokens)
{
	const char* command = tokens->at(0).c_str();
	Command result = None;
	// Comparings
	if (strcmp(command, "list") == 0 &&
		tokens->size() == 1)
	{
		result = CMD_LIST;
	}
	if (strcmp(command, "create") == 0 &&
		tokens->size() == 2)
	{
		result = CMD_CREATE;
	}
	if (strcmp(command, "drop") == 0 &&
		tokens->size() == 2)
	{
		result = CMD_DROP;
	}
	if (strcmp(command, "truncate") == 0 &&
		tokens->size() == 2)
	{
		result = CMD_TRUNCATE;
	}
	if (strcmp(command, "select") == 0 &&
		tokens->size() == 2)
	{
		result = CMD_SELECT;
	}
	if (strcmp(command, "insert") == 0 &&
		tokens->size() == 3)
	{
		result = CMD_INSERT;
	}
	if (strcmp(command, "delete") == 0 &&
		tokens->size() == 3)
	{
		result = CMD_DELETE;
	}
	if (strcmp(command, "update") == 0 &&
		tokens->size() == 4)
	{
		result = CMD_UPDATE;
	}
	if (strcmp(command, "help") == 0 &&
		tokens->size() == 1)
	{
		result = CMD_HELP;
	}
	if (strcmp(command, "quit") == 0 &&
		tokens->size() == 1)
	{
		result = CMD_QUIT;
	}
	return result;
}

std::string CommandExecutor::RunFileProcessing(Command command, std::vector<std::string>* tokens)
{
	std::string result;
	switch (command)
	{
	case CMD_LIST:
		result = _file_processing->List();
		break;
	case CMD_CREATE:
		result = _file_processing->Create(tokens->at(1));
		break;
	case CMD_DROP:
		result = _file_processing->Drop(tokens->at(1));
		break;
	case CMD_TRUNCATE:
		result = _file_processing->Truncate(tokens->at(1));
		break;
	case CMD_SELECT:
		result = _file_processing->Select(tokens->at(1));
		break;
	case CMD_INSERT:
		result = _file_processing->Insert(tokens->at(1), tokens->at(2));
		break;
	case CMD_DELETE:
		result = _file_processing->Delete(tokens->at(1), tokens->at(2));
		break;
	case CMD_UPDATE:
		result = _file_processing->Update(tokens->at(1), tokens->at(2), tokens->at(3));
		break;
	case CMD_QUIT:
		result = "Buy buy ...";
		break;
	case CMD_HELP:
		result =
			"Commands information (MySQL):\r\n\
 - List (show tables)\r\n\
 - Select argv (SELECT * FROM 'argv';)\r\n\
 - Create argv (CREATE TABLE 'argv';)\r\n\
 - Drop argv (DROP TABLE 'argv';)\r\n\
 - Truncate argv (TRUNCATE TABLE 'argv';)\r\n\
 - Insert argv value (INSERT INTO 'argv' VALUES ('value');)\r\n\
 - Delete argv value (DELETE FROM 'argv' value='value';)\r\n\
 - Update argv value new_value (UPDATE 'argv' SET value='new_value' WHERE value='value';)\r\n\
 - Help(commands information)\r\n\
 - Quit(exit)";
		break;
	case None:
		result = "Unsuitable command. Use 'help' for more information";
		break;
	}
	return result;
}