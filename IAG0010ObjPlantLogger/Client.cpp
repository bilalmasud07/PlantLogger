#include "stdafx.h"
#include "Client.h"

using namespace std; 

Client::Client()
{
	//_tcout << "Client Class Initialized" << endl;
	WSADATA	WsaData;
	int Error = WSAStartup(MAKEWORD(2, 2), &WsaData);

	if (Error)
	{
		_tcout << "Unable to load Winsock: " << Error << endl;
		exit(0);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	addr.sin_port = htons(SERVER_PORT);
}


Client::~Client()
{
}


SOCKET	Client::GetSocket()
{
	return this->ConnectSocket;
}


bool Client::Valid()
{
	SOCKET& Conn = this->ConnectSocket;
	if (Conn == INVALID_SOCKET)
		return FALSE;
	return true;
}


bool Client::Connect()
{
	// create a shorter alias for class ConnectSocket bcos I can!
	SOCKET& Conn = this->ConnectSocket;

	// initiate connection
	Conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Conn == INVALID_SOCKET)
		return FALSE;

	if (connect(Conn, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
		return FALSE;

	char RecvBuf[128] = { 0 };
	if (recv(Conn, RecvBuf, MAX_BYTE, 0) <= 0) // check server response
		return FALSE;

	// send password
	if (this->Send(PASSWORD))
	{
		if (recv(Conn, RecvBuf, MAX_BYTE, 0) <= 0) // check server response
			return FALSE;
	}

	return TRUE;
}


void Client::Disconnect()
{
	// create a shorter alias for class ConnectSocket bcos I can!
	SOCKET& Conn = this->ConnectSocket;
	// clear sockets
	if (Conn != INVALID_SOCKET)
	{
		closesocket(Conn);
		//WSACleanup();
	}
	//this->Connected = false;
}


int Client::_Send(const TCHAR* data)
{
	SOCKET& Conn = this->ConnectSocket;

	Packet Packet(data);
	int SentBytes = 0;

	while (SentBytes < Packet.getNumberOfBytes())
	{
		int res = send(Conn, (char*)&Packet + SentBytes, 
						Packet.getNumberOfBytes() - SentBytes, 0);
		if (res == SOCKET_ERROR)
		{
			throw exception();
			//_tcout << L"WSASendxx failed with error: " << WSAGetLastError() << endl;
			break;
		}
		SentBytes += res; 
	}

	return SentBytes;
}

int Client::Send(const TCHAR* data)
{
	try {
		return this->_Send(data);
	}
	catch (exception e)
	{
		_tcout << "Sending failed." << endl;
	}
	return 0;
}

/*int Client::Send(const TCHAR* data)
{
	Packet Packet(data);

	WSAOVERLAPPED SendOverlapped;
	WSABUF	DataBuf;
	DWORD	SendBytes;
	DWORD	Flags;

	int rc;
	int err = 0;

	SecureZeroMemory((PVOID)& SendOverlapped, sizeof(WSAOVERLAPPED));
	SendOverlapped.hEvent = WSACreateEvent();
	if (SendOverlapped.hEvent == NULL)
	{
		_tcout << "SendOverlapped failed with error: " << WSAGetLastError() << endl;
		return 0;
	}

	DataBuf.len = Packet.getNumberOfBytes();
	DataBuf.buf = (char*)&Packet;

	SOCKET& Conn = this->ConnectSocket;

	rc = WSASend(Conn, &DataBuf, 1, &SendBytes, 0, &SendOverlapped, NULL);
	if ((rc == SOCKET_ERROR) && (WSA_IO_PENDING != (err = WSAGetLastError())))
	{
		_tcout << L"WSASend failed with error: " << err << endl;
		return 0;
	}

	rc = WSAWaitForMultipleEvents(1, &SendOverlapped.hEvent, 
			FALSE, INFINITE, TRUE);

	if (rc == WSA_WAIT_FAILED)
	{
		WSAResetEvent(SendOverlapped.hEvent);
		_tcout << L"Send WSAWaitForMultipleEvents failed with error: " 
			   << WSAGetLastError() << endl;
		return 0;
	}

	// if SendOverlapped.hEvent was signaled.
	rc = WSAGetOverlappedResult(Conn, &SendOverlapped, 
			&SendBytes, FALSE, &Flags);
	if (rc == FALSE)
	{
		_tcout << L"WSASend failed with error: %d\n" << WSAGetLastError() 
			   << endl;
		return 0;
	}

	return SendBytes;
} */


/*int Client::Recv(char* recvBuf)
{
	WSAOVERLAPPED RecvOverlapped;
	WSABUF DataBuf;
	DWORD Result, RecvBytes, Flags = 0;
	char buffer[MAX_BYTE];

	int rc;
	int err = 0;

	SecureZeroMemory((PVOID)& RecvOverlapped, sizeof(WSAOVERLAPPED));
	RecvOverlapped.hEvent = WSACreateEvent();
	if (RecvOverlapped.hEvent == NULL) {
		_tcout << "RecvOverlapped failed with error: " << WSAGetLastError() 
			   << endl;
		return 0;
	}

	DataBuf.len = MAX_BYTE;
	DataBuf.buf = buffer;

	SOCKET& Conn = this->ConnectSocket;

	while (1)
	{
		Result = WSARecv(Conn, &DataBuf, 1, &RecvBytes, &Flags,
			&RecvOverlapped, NULL);
		if (Result == SOCKET_ERROR)
		{
			if (WSA_IO_PENDING != (err = WSAGetLastError())) {
				wprintf(L"Connection broken: %d\n", err);
				break;
			}

			rc = WSAWaitForMultipleEvents(1, &RecvOverlapped.hEvent, FALSE,
				WSA_INFINITE, FALSE);

			WSAResetEvent(RecvOverlapped.hEvent);
			if (rc == WSA_WAIT_FAILED)
			{
				_tcout << L"Recv WSAWaitForMultipleEvents failed with error: "
					<< WSAGetLastError() << endl;
				break;
			}
			else if (rc == WAIT_OBJECT_0 + 1)
			{
				// Waiting stopped because RecvOverlapped.hEvent is now signaled, 
				// i.e. the receiving operation has ended. 
				// Now we have to see how many bytes we have got.

				if (WSAGetOverlappedResult(Conn, &RecvOverlapped,
					&RecvBytes, FALSE, &Flags))
				{
					wprintf(L"Read bytes: %d\n", RecvBytes);

					return RecvBytes;
				}
				else
				{
					wprintf(L"WSARecv operation failed with error: %d\n",
						WSAGetLastError());
					return -3;
				}
			}
		}
		else
		{  // Returned immediately without socket error
			if (!RecvBytes)
			{  // When the receiving function has read nothing and 
			   // returned immediately, the connection is off  
				_tprintf(_T("Server has closed the connection\n"));
				return 0;
			}
			else
			{
				_tprintf(_T("%d bytes received - 2\n"), RecvBytes);
			}
		}
	}

	return err;
}*/
