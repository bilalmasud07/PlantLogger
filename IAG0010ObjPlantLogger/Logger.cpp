#include "stdafx.h"
#include "Logger.h"
#include <string>

using namespace std;

Logger::Logger()
{
	//_tcout << "Logger Class Initialized" << endl;
	this->EMULATOR_NOT_RESP = "Emulator not responding.";
}


Logger::~Logger()
{
}


void Logger::Keyboard(Client* client, Command* command)
{
	while (!this->Quit)
	{
		//lock_guard<mutex> guard(this->SendLock);
		TCHAR enteredCommand[10];
		_tcin.getline(enteredCommand, 10);

		// prevent buffer overflow
		if (_tcin.fail())
		{
			_tcin.clear();
			continue;
		}
		command->Set(enteredCommand);

		if (!command->IsValid())
		{
			_tcout << "Enter a valid command" << endl;
			continue;
		}

		// if valid command
		if (command->IsExit())
		{
			this->Exit(client);
		}
		else
		{
			this->BeginSend = true;
			this->CommandEntered.notify_one();
		}
	}
	//_tcout << "Closing keyboard thread" << endl;
}


void Logger::Send(Client* client, Command* command)
{
	while (1)
	{
		unique_lock<mutex> locker(this->SendLock);
		this->CommandEntered.wait(locker, [this]() {
			// wake the thread if valid command was entered or quit
			return BeginSend || Quit; 
		});

		if (this->Quit)
			break;

		// send lock signaled.
		if (command->IsConnect())
		{
			// ignore connect command if logger is already connected.
			if (client->Valid() && this->Accepted)
				continue;

			// else
			_tcout << "Initiating connection..." << endl;
			if (client->Connect())
			{
				_tcout << "Connected!" << endl;
				this->Accepted = true;
			}
			else
				cout << this->EMULATOR_NOT_RESP << endl;
		}
		else
		{
			// ignore all cmds if logger is not connected with emulator.
			if (!this->Accepted)
			{
				_tcout << "Type \"connect\" to begin."
					   << endl;
			}
			else
			{
				int SentBytes = 1; // if SentBytes is 0 then there was an error

				// if the cmd is stop
				if (command->IsStop() && this->Accepted)
				{
					SentBytes = client->Send(EMULATOR_CMD_STOP);
					this->OnBreak = false;
					this->Receiving = false;
					this->Accepted = false;
				}
				// if cmd is break and logger is receiving and not already on break
				if (command->IsBreak() && this->Receiving && !this->OnBreak)
				{
					SentBytes = client->Send(EMULATOR_CMD_BREAK);
					this->OnBreak = true;
				}
				// if the cmd is start and logger is not already receiving
				if (command->IsStart() && (!this->Receiving || this->OnBreak))
				{
					SentBytes = client->Send(EMULATOR_CMD_START);
					this->OnBreak = false;
					this->Receiving = true;
				}
				// if recv thread sends the "Ready" command
				if (!_tcsicmp(command->Get(), EMULATOR_CMD_READY))
				{
					SentBytes = client->Send(EMULATOR_CMD_READY);
					this->Receiving = true;
				}

				if (!SentBytes)
				{
					this->Receiving = false;
					this->Accepted = false;
					this->OnBreak = false;

					cout << this->EMULATOR_NOT_RESP << endl;
					client->Disconnect();
				}
				
				if (this->Receiving)
				{
					// wake recv thread to start receiving data
					this->RecvBegin.notify_one();
				}
			}
		}

		/*_tcout << "Accepted: " << this->Accepted 
			   << " Receiving: " << this->Receiving 
			   << " On Break: " << this->OnBreak << endl;*/

		//locker.unlock();
		this->BeginSend = false;
	}

	this->SendDone = true;
	//_tcout << "Closing Send thread" << endl;
}


void Logger::Recv(Client* client, Command* command, fstream* logfile)
{
	while (1)
	{
		unique_lock<mutex> locker(this->RecvLock);
		this->RecvBegin.wait(locker, [this]() {
			// wake the thread if valid command was entered or quit
			return Receiving || Quit;
		});

		if (this->Quit)
			break;

		// recv lock signaled.
		SOCKET Conn = client->GetSocket();

		// timeouts the recv if no data is received.
		int Timeout = 500;
		setsockopt(Conn, SOL_SOCKET, SO_RCVTIMEO, (char*)&Timeout, sizeof(int));

		char RecvBuf[1024];
		int rc = recv(Conn, RecvBuf, 1024, 0);
		int err = WSAGetLastError();

		if (rc > 0)    // handle this data 
		{
			_tcout << left << setw(4) << rc << "bytes received from emulator" << endl;
			if (this->Receiving)
				this->Write(logfile, RecvBuf, rc);
		}
		else
		{
			if (rc < 0 && (err == WSAEINTR ||
							err == WSAEWOULDBLOCK || 
							err == WSAETIMEDOUT))
			{
				continue;   // ignore the timeout error 
			}
			else
			{
				client->Disconnect();
			}
		}

		// send ready command
		command->Set(L"Ready");
		this->BeginSend = true;
		this->CommandEntered.notify_one();
	}

	this->RecvDone = true;
	//_tcout << "Closing Recv thread" << endl;
}


void Logger::Exit(Client* client)
{
	_tcout << "Shutting down..." << endl;

	// quit all threads
	this->Quit = true;

	// notify send thread and recv thread
	this->CommandEntered.notify_one();
	this->RecvBegin.notify_one();

	// disconnect from emulator
	if (this->SendDone && this->RecvDone)
		client->Disconnect();

	_tcout << "Disconnected from emulator." << endl;
}


int Logger::Write(fstream* logfile, char* recvbuf, int len)
{
	struct mPackage
	{
		int len;
		int numberOfChannels;
		char data[1024];
	};

	mPackage* Package = (mPackage*)recvbuf;

	if (Package->len != len)
		return 1;

	char FileBuf[2048];

	// get the time
	time_t CurrentTime;
	time(&CurrentTime);
	struct tm* MyTime = localtime(&CurrentTime);
	// format time
	char TimeBuf[24];
	strftime(TimeBuf, 24, "%Y-%m-%d %H:%M:%S", MyTime);
	// write time to file buffer
	sprintf_s(FileBuf, "\r\nMeasurement results at %s\r\n", TimeBuf);
	this->Print(logfile, FileBuf);

	char* buffer = Package->data;
	char* ChannelName = { 0 };

	for (int i = 0; i < Package->numberOfChannels; i++)
	{
		int NumberOfPoints = *((int*)buffer);
		buffer += 4;
		//int NumberOfPoints;
		//memcpy(&NumberOfPoints, Channel, 4);
		ChannelName = buffer;
		sprintf_s(FileBuf, "%s:\r\n", ChannelName);
		this->Print(logfile, FileBuf);
		buffer += (strlen(ChannelName) + 1); // include terminating null

		char* PointName = { 0 };
		int iValue = 0;
		double dValue = 0.0;

		for (int j = 0; j < NumberOfPoints; j++)
		{
			PointName = buffer;
			//printf("%s", PointName);
			sprintf_s(FileBuf, "%s: ", PointName);
			this->Print(logfile, FileBuf);
			buffer += (strlen(PointName) + 1);

			//_strlwr(PointName);	// make point lowercase
			// check if next byte is int or double
			if (strcmp(PointName, "Level") == 0 ||
				strcmp(PointName, "Acid on output") == 0)
			{
				iValue = *((int*)buffer);
				//printf("%d\n", iValue);
				sprintf_s(FileBuf, "%d%%\r\n", iValue);
				this->Print(logfile, FileBuf);
				buffer += 4; // move the point (int size) places
			}
			else
			{
				dValue = *((double*)buffer);
				if (strcmp(PointName, "Temperature") == 0 ||
					strcmp(PointName, "Input temperature") == 0)
				{
					sprintf_s(FileBuf, "%.4f\xB0\x43\r\n", dValue);
				}
				if (strcmp(PointName, "Pressure") == 0 ||
					strcmp(PointName, "Output pressure") == 0)
				{
					sprintf_s(FileBuf, "%.5fatm\r\n", dValue);
				}
				if (strcmp(PointName, "Input pressure") == 0)
				{
					sprintf_s(FileBuf, "%.6fatm\r\n", dValue);
				}
				if (strcmp(PointName, "Output temperature") == 0)
				{
					sprintf_s(FileBuf, "%.3f\xB0\x43\r\n", dValue);
				}
				//printf("%f\n", dValue);
				this->Print(logfile, FileBuf);
				buffer += 8; // move the point (double size) places
			}
		}
	}

	return 0;
}

void Logger::Print(fstream* logfile, char* buffer)
{
	*logfile << buffer;
	logfile->flush();	// writes to file immediately incase program crashes
	cout << buffer;
}
