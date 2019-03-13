#include "..\inc\InputSocketBufferer.h"
#include <exception>


InputSocketBufferer::InputSocketBufferer(TCPSocket* socket)
	:
	SocketBufferer(socket)
{
}

void InputSocketBufferer::recieve()
{
	SocketBufferer::recieve();
}

UINT InputSocketBufferer::find(const BYTE* const src, UINT length)
{
	return this->findInBuffer(0, src, length);
}

UINT InputSocketBufferer::findFromPos(UINT pos, const BYTE * const src, UINT length)
{
	return this->findInBuffer(pos, src, length);
}

UINT InputSocketBufferer::read(BYTE* buffer, UINT length)
{
	return this->readBuffer(buffer, length);
}
