#include "..\inc\SocketBufferer.h"
#include <exception>


SocketBufferer::SocketBufferer(TCPSocket* socket)
{
	this->socket = socket;
	this->buffer = (BYTE*)malloc(sizeof(BYTE));
	this->bufferLength = 1;
	this->bufferContentLength = 0;
}


SocketBufferer::~SocketBufferer()
{
	free(this->buffer);
}

void SocketBufferer::writeBuffer(const BYTE* src, UINT srcLength)
{
	while (this->bufferLength < (this->bufferContentLength + srcLength))
	{
		this->bufferLength *= 2;
		this->buffer = (BYTE*)realloc(this->buffer, this->bufferLength);
		if (!this->buffer) throw new std::exception("realloc failed");
	}

	for (UINT i = 0; i < srcLength; i++)
	{
		this->buffer[this->bufferContentLength] = src[i];
		this->bufferContentLength++;
	}
}

UINT SocketBufferer::readBuffer(BYTE * dest, UINT destLength)
{
	if (destLength == 0) return 0;


	const UINT outputCount = this->bufferContentLength > destLength ? destLength : this->bufferContentLength;
	if (dest && destLength)
	{
		for (UINT i = 0; i < outputCount; i++)
		{
			dest[i] = this->buffer[i];
		}
	}

	const UINT startOffsetNow = outputCount;
	for (UINT i = 0; i < this->bufferContentLength - startOffsetNow; i++)
	{
		this->buffer[i] = this->buffer[i + startOffsetNow];
	}

	this->bufferContentLength -= outputCount;

	return outputCount;
}

UINT SocketBufferer::findInBuffer(UINT pos, const BYTE * const src, UINT length)
{
	UINT curPos = pos;
	UINT matched = 0;
	while (this->bufferContentLength > curPos)
	{
		if (matched < length)
		{
			if (this->buffer[curPos] == src[matched])
			{
				matched++;
			}
			else
			{
				matched = 0;
			}
		}
		else
		{
			return curPos - length;
		}
		curPos++;
	}
	if (matched == length)
	{
		return curPos - length;
	}
	else
	{
		return -1;
	}
}

void SocketBufferer::recieve()
{
	BYTE socketBuffer[this->SOCKET_BUFFER_LENGTH];
	int recieved = this->socket->recvData(socketBuffer, this->SOCKET_BUFFER_LENGTH);
	this->writeBuffer(socketBuffer, recieved);
}

void SocketBufferer::send()
{
	BYTE socketBuffer[this->SOCKET_BUFFER_LENGTH];
	UINT readBytes = 0;
	while (readBytes = this->readBuffer(socketBuffer, this->SOCKET_BUFFER_LENGTH))
	{
		this->socket->sendData(socketBuffer, readBytes);
	}
}
