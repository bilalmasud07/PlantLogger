#ifndef PACKET_H
#define PACKET_H

#include <string.h>

class Packet
{
public:
	Packet(const TCHAR*);
	~Packet();
	int		getNumberOfBytes();

private:
	int		NumberOfBytes;
	TCHAR	Data[1024];
};

#endif
