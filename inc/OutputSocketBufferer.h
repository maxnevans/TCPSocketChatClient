#pragma once
#include "SocketBufferer.h"

class OutputSocketBufferer
	:
	SocketBufferer
{
public:
	OutputSocketBufferer(TCPSocket* socket);
	void write(const BYTE* const src, UINT length);
	void send();

private:
};

