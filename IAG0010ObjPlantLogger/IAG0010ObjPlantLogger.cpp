// IAG0010ObjPlantLogger.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Command.h"
#include "Client.h"
#include "Logger.h"
#include <thread>
#include <fstream>

using namespace std;

int _tmain(int argc, _TCHAR *argv[])
{
	if (argc == 1)
	{
		_tcout << "Name of file missing." << endl
			   << "Usage: " << argv[0] << " [name-of-file.txt]\n" << endl;
		return 1;
	}

	fstream LoggerFile;
	LoggerFile.open(argv[1], fstream::in | fstream::out | fstream::app);
	if (!LoggerFile.good())
	{
		_tcout << "Could not create file." << strerror(errno) << "." << endl;
		return 1;
	}

	// Welcome message
	_tcout << "Welcome to IAG0010ObjPlantLogger" << endl;
	_tcout << "Type \"connect\" to begin." << endl << endl;

	Command Cmd;
	Client LoggerClient;
	Logger PlantLogger;

	thread Keyboard(&Logger::Keyboard, ref(PlantLogger), &LoggerClient, &Cmd);
	thread Send(&Logger::Send, ref(PlantLogger), &LoggerClient, &Cmd);
	thread Recv(&Logger::Recv, ref(PlantLogger), &LoggerClient, &Cmd, &LoggerFile);

	Keyboard.join();
	Send.join();
	Recv.join();

	LoggerFile.close();

	//system("pause");
    return 0;
}

