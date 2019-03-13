#pragma once
#include "TCPSocket.h"

class SocketBufferer
{
public:
	SocketBufferer(TCPSocket* socket);
	~SocketBufferer();

protected:
	void writeBuffer(const BYTE* src, UINT srcLength);
	UINT readBuffer(BYTE* dest, UINT destLength);
	UINT findInBuffer(UINT pos, const BYTE* const src, UINT length);
	virtual void recieve();
	virtual void send();

private:
	static constexpr UINT SOCKET_BUFFER_LENGTH = 0xFF;

	UINT bufferLength;
	UINT bufferContentLength;
	BYTE* buffer;
	TCPSocket* socket;
};
