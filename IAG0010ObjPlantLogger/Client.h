#ifndef CLIENT_H
#define CLIENT_H

#include "WinSock2.h"
#include "mswsock.h"
#include "Packet.h"
#include <exception>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma warning(disable : 4290) 
#pragma warning(disable : 4996)

class Client
{
public:
	Client();
	~Client();
	SOCKET	GetSocket();
	bool	Valid();
	bool	Connect();		// initiate connection to the server
	//bool IsConnected();	// checks if client is connected to server
	void	Disconnect();	// disconnects client from server
	int		Send(const TCHAR*);
	//int		Recv(char*);	// recv from the server, returns no of bytes

private:
	int		_Send(const TCHAR*);	// sends packets to server (throws exception)

private:
	static const int MAX_BYTE = 1024; // Max byte to receive from emulator
	const char*	SERVER_ADDR = "127.0.0.1";
	const int	SERVER_PORT = 1234;
	const TCHAR* PASSWORD = L"coursework";

	SOCKET		ConnectSocket; // Client socket used to connect
	sockaddr_in addr;	// address binder to socket
	int			sizeofaddr = sizeof(addr);
	char*		ServerAddr;	// Address of the server
	int			ServerPort;	// Port to connect to on server

	//bool Connected = false;
};

#endif

