#include "..\inc\MessageBufferer.h"
#include <assert.h>
#include <exception>

MessageBufferer::MessageBufferer(const char* remoteAddr, const char* remotePort)
{
	this->socket = new TCPSocket(remoteAddr, remotePort);
	this->isbufferer = new InputSocketBufferer(this->socket);
	this->osbufferer = new OutputSocketBufferer(this->socket);
}

MessageBufferer::~MessageBufferer()
{
	delete this->isbufferer;
	delete this->osbufferer;
	delete this->socket;
}

bool MessageBufferer::tryMessage(Message* msg)
{
	assert(msg != nullptr);
	
	UINT startIndex = this->isbufferer->find(
		(BYTE*)&this->MESSAGE_START_SEPARATOR,
		sizeof(this->MESSAGE_START_SEPARATOR)
	);
	if (startIndex != 0 && startIndex != -1)
	{
		throw new std::exception("MessageStream: sequence violated");
	}
	UINT endIndex = this->isbufferer->find(
		(BYTE*)&this->MESSAGE_END_SEPARATOR, 
		sizeof(this->MESSAGE_END_SEPARATOR)
	);
	if (endIndex == -1)
	{
		return false;
	}
	UINT nextStartIndex = this->isbufferer->findFromPos(
		endIndex, 
		(BYTE*)&this->MESSAGE_START_SEPARATOR, 
		sizeof(this->MESSAGE_START_SEPARATOR)
	);
	if (nextStartIndex != -1)
	{
		while (!((nextStartIndex - endIndex) != sizeof(this->MESSAGE_END_SEPARATOR)))
		{
			endIndex = this->isbufferer->findFromPos(
				nextStartIndex, 
				(BYTE*)&this->MESSAGE_END_SEPARATOR, 
				sizeof(this->MESSAGE_END_SEPARATOR)
			);
			if (endIndex == -1) return false;
			nextStartIndex = this->isbufferer->findFromPos(
				endIndex, 
				(BYTE*)&this->MESSAGE_START_SEPARATOR, 
				sizeof(this->MESSAGE_START_SEPARATOR)
			);
			if (nextStartIndex == -1) break;
		}
	}

	this->isbufferer->read(nullptr, sizeof(this->MESSAGE_START_SEPARATOR));
	ULONG bufferLength = (endIndex - sizeof(this->MESSAGE_START_SEPARATOR)) * sizeof(BYTE);
	BYTE* buffer = (BYTE*)malloc(bufferLength);
	if (!buffer) throw new std::exception("malloc failed");
	UINT msgBytesWrote = this->isbufferer->read(buffer, bufferLength);
	msg->from(buffer, bufferLength);
	this->isbufferer->read(nullptr, sizeof(this->MESSAGE_END_SEPARATOR));

	return true;
}

void MessageBufferer::sendMessage(Message * const msg, bool sendNow)
{
	this->osbufferer->write((BYTE*)&this->MESSAGE_START_SEPARATOR, sizeof(this->MESSAGE_START_SEPARATOR));
	this->osbufferer->write((BYTE*)msg->getBuffer(), msg->getBufferLength());
	this->osbufferer->write((BYTE*)&this->MESSAGE_END_SEPARATOR, sizeof(this->MESSAGE_END_SEPARATOR));

	if (sendNow)
	{
		this->osbufferer->send();
	}
}

void MessageBufferer::recieve()
{
	this->isbufferer->recieve();
}

void MessageBufferer::send()
{
	this->osbufferer->send();
}
