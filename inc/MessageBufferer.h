#pragma once
#include "InputSocketBufferer.h"
#include "OutputSocketBufferer.h"
#include "Message.h"

class MessageBufferer
{
public:
	MessageBufferer(const char* remoteAddr, const char* remotePort);
	bool tryMessage(Message* msg);
	void sendMessage(Message* const msg, bool sendNow = false);
	void recieve();
	void send();
	~MessageBufferer();

private:
	static constexpr unsigned __int32 MESSAGE_START_SEPARATOR	= 0xFF12AABB;
	static constexpr unsigned __int32 MESSAGE_END_SEPARATOR		= 0xBBCCCCCC;

	InputSocketBufferer* isbufferer;
	OutputSocketBufferer* osbufferer;
	TCPSocket* socket;
};

