#ifndef COMMAND_H
#define COMMAND_H

class Command
{
public:
	Command();
	~Command();

	void	Set(TCHAR*);	// set the value of the entered command
	TCHAR*	Get();
	bool	IsBreak();
	bool	IsConnect();
	bool	IsExit();
	bool	IsStart();
	bool	IsStop();
	bool	IsValid();	// returns true if the entered command is valid

private:
	const TCHAR* LOGGER_CMD_BREAK = L"break";
	const TCHAR* LOGGER_CMD_CONNECT = L"connect";
	const TCHAR* LOGGER_CMD_EXIT = L"exit";
	const TCHAR* LOGGER_CMD_START = L"start";
	const TCHAR* LOGGER_CMD_STOP = L"stop";

	TCHAR* EnteredCommand;	// holds the cmd entered by the user.
};

#endif
