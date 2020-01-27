#include "stdafx.h"
#include "Command.h"

using namespace std;

Command::Command()
{
	//_tcout << "Command Class Initialized" << endl;
}


Command::~Command()
{
}


void Command::Set(TCHAR* cmd)
{
	this->EnteredCommand = cmd;
}


TCHAR* Command::Get()
{
	return this->EnteredCommand;
}


bool Command::IsBreak()
{
	return !_tcsicmp(this->EnteredCommand, this->LOGGER_CMD_BREAK);
}


bool Command::IsConnect()
{
	return !_tcsicmp(this->EnteredCommand, this->LOGGER_CMD_CONNECT);
}


bool Command::IsExit()
{
	return !_tcsicmp(this->EnteredCommand, this->LOGGER_CMD_EXIT);
}


bool Command::IsStart()
{
	return !_tcsicmp(this->EnteredCommand, this->LOGGER_CMD_START);
}


bool Command::IsStop()
{
	return !_tcsicmp(this->EnteredCommand, this->LOGGER_CMD_STOP);
}


bool Command::IsValid()
{
	return (!_tcsicmp(this->EnteredCommand, LOGGER_CMD_CONNECT) ||
			!_tcsicmp(this->EnteredCommand, LOGGER_CMD_START) ||
			!_tcsicmp(this->EnteredCommand, LOGGER_CMD_BREAK) ||
			!_tcsicmp(this->EnteredCommand, LOGGER_CMD_STOP) ||
			!_tcsicmp(this->EnteredCommand, LOGGER_CMD_EXIT));
}
