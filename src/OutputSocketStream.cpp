#include "..\inc\OutputSocketBufferer.h"

OutputSocketBufferer::OutputSocketBufferer(TCPSocket* socket)
	:
	SocketBufferer(socket)
{
}

void OutputSocketBufferer::write(const BYTE * const src, UINT length)
{
	this->writeBuffer(src, length);
}

void OutputSocketBufferer::send()
{
	SocketBufferer::send();
}
