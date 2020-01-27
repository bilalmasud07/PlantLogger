#ifndef LOGGER_H
#define LOGGER_H

#include "Command.h"
#include "Client.h"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <iomanip>

class Logger
{
public:
	Logger();
	~Logger();

	void Keyboard(Client*, Command*); // Keyboard thread: accept commands from user.
	void Send(Client*, Command*);	// Sender thread: sends data to the server.
	void Recv(Client*, Command*, std::fstream*); // Receive Thread: receives data from the server.
	void Exit(Client*); // stop all logger operations.
	int	 Write(std::fstream*, char*, int); // write logger data to a file

private:
	void Print(std::fstream*, char*);	//

private:
	std::string  EMULATOR_NOT_RESP;

	const TCHAR* EMULATOR_CMD_BREAK = L"Break";
	const TCHAR* EMULATOR_CMD_READY = L"Ready";
	const TCHAR* EMULATOR_CMD_START = L"Start";
	const TCHAR* EMULATOR_CMD_STOP = L"Stop";

	std::atomic<bool> Accepted = false;	// is true when emulator accepts connection
	std::atomic<bool> Receiving = false;	// is true when receiving begins
	std::atomic<bool> OnBreak = false;	// is true when logging is on break
	std::mutex SendLock;
	std::condition_variable CommandEntered;
	std::mutex RecvLock;
	std::condition_variable RecvBegin;

	bool Quit = false;
	bool BeginSend = false;	// prevent spurious wake of send thread
	bool SendDone = false;	// signals end of send thread
	bool RecvDone = false;	// signals end of recv thread
};

#endif

