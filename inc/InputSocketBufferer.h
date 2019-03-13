#pragma once
#include "SocketBufferer.h"
#include "TCPSocket.h"

class InputSocketBufferer
	:
	public SocketBufferer
{
public:
	InputSocketBufferer(TCPSocket* socket);
	void recieve();
	UINT find(const BYTE* const src, UINT length);
	UINT findFromPos(UINT pos, const BYTE* const src, UINT length);
	UINT read(BYTE* buffer, UINT length);
};

