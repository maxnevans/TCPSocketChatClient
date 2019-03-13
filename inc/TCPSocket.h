#pragma once
#include <winsock2.h>

typedef unsigned char BYTE;

class TCPSocket
{
public:
	TCPSocket(const char* remoteAddr, const char* remotePort);
	void sendData(const BYTE* data, int length);
	int recvData(BYTE* buffer, int bufferLength);
	~TCPSocket();

private:
	SOCKET ConnectSocket;
};

