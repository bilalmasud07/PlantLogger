#include "stdafx.h"
#include "Packet.h"


Packet::Packet(const TCHAR* pString)
{
	int pStringLength = _tcslen(pString) + 1; // include terminating null
	int stringByteLength = pStringLength * sizeof(TCHAR);

	this->NumberOfBytes = 4 + stringByteLength;
	wcscpy_s(this->Data, pString);
}


Packet::~Packet()
{
}


int Packet::getNumberOfBytes()
{
	return this->NumberOfBytes;
}
