#include "..\inc\Message.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <exception>

Message::Message(const char* type, const char* sign, const BYTE* content, UINT contentLength)
{
	this->to(type, sign, content, contentLength);
}

Message::Message(const BYTE* message, ULONG length)
{
	this->from(message, length);
}

Message::Message()
{
	this->action = nullptr;
	this->sign = nullptr;
	this->content = nullptr;
	this->buffer = nullptr;
	this->actionLength = 0;
	this->signLength = 0;
	this->contentLength = 0;
	this->bufferLength = 0;
}

void Message::from(const BYTE* message, ULONG length)
{
	free(this->action);
	free(this->sign);
	free(this->content);
	free(this->buffer);
	this->action = nullptr;
	this->sign = nullptr;
	this->content = nullptr;
	this->buffer = nullptr;
	this->actionLength = 0;
	this->signLength = 0;
	this->contentLength = 0;
	this->bufferLength = 0;

	const BYTE* ptr = message;
	int ptrIndex = 0;

	try
	{
		//Action parse
		this->actionLength = 0;
		while (*(ptr++) != this->delim) this->actionLength++;
		ptr += this->delimRepeat - 1;
		if (this->actionLength > 0)
		{
			this->action = (char*)malloc(this->actionLength + 1);
			if (this->action == nullptr) throw new std::exception("malloc fail!");

			for (int i = 0; i < this->actionLength; i++)
				this->action[i] = message[i];
			this->action[this->actionLength] = 0;
		}
		ptrIndex += this->actionLength + this->delimRepeat;

		//Sign parse
		this->signLength = 0;
		while (*(ptr++) != this->delim) this->signLength++;
		ptr += this->delimRepeat - 1;
		if (this->signLength > 0)
		{
			this->sign = (char*)malloc(this->signLength + 1);
			if (this->sign == nullptr) throw new std::exception("malloc fail!");

			for (int i = 0; i < this->signLength; i++)
				this->sign[i] = message[ptrIndex + i];
			this->sign[this->signLength] = 0;
		}
		ptrIndex += this->signLength + this->delimRepeat;

		//Content parse
		this->contentLength = length - ptrIndex;
		if (this->contentLength > 0)
		{
			this->content = (BYTE*)malloc(this->contentLength);
			if (this->content == nullptr) throw new std::exception("malloc fail!");

			for (ULONG i = 0; i < this->contentLength; i++)
				this->content[i] = message[ptrIndex + i];
			this->content[this->contentLength] = 0;
		}

	}
	catch (...)
	{
		free(this->action);
		free(this->sign);
		free(this->content);
		this->action = nullptr;
		this->sign = nullptr;
		this->content = nullptr;
		this->actionLength = 0;
		this->signLength = 0;
		this->contentLength = 0;
		throw;
	}
}

void Message::to(const char* type, const char* sign, const BYTE* content, UINT contentLength)
{
	this->action = nullptr;
	this->sign = nullptr;
	this->content = nullptr;
	this->buffer = nullptr;
	this->actionLength = 0;
	this->signLength = 0;
	this->contentLength = 0;
	this->bufferLength = 0;

	this->setAction(type);
	this->setSign(sign);
	this->setContent(content, contentLength);
}

void Message::setAction(const char* action)
{
	free(this->action);
	this->actionLength = (int)strlen(action);
	this->action = (char*)malloc(this->actionLength + 1);
	strcpy_s(this->action, this->actionLength + 1, action);
}

void Message::setSign(const char* sign)
{
	free(this->sign);
	this->signLength = (int)strlen(sign);
	this->sign = (char*)malloc(this->signLength + 1);
	strcpy_s(this->sign, this->signLength + 1, sign);
}

void Message::setContent(const BYTE* content, UINT contentLength)
{
	free(this->content);
	this->content = nullptr;
	this->contentLength = contentLength;
	if (this->contentLength > 0)
	{
		this->content = (BYTE*)malloc(this->contentLength * sizeof(BYTE));
		for (UINT i = 0; i < this->contentLength; i++)
		{
			this->content[i] = content[i];
		}
	}
}

const BYTE* Message::getBuffer()
{
	if (this->bufferLength != (this->actionLength + this->bufferLength + this->contentLength))
		this->updateBuffer();
	return this->buffer;
}

int Message::getBufferLength()
{
	if (this->bufferLength != (this->actionLength + this->bufferLength + this->contentLength))
		this->updateBuffer();
	return this->bufferLength;
}

void Message::updateBuffer()
{
	free(this->buffer);
	this->bufferLength = this->actionLength + this->contentLength 
		+ this->signLength + sizeof(this->delim)*this->delimRepeat*(this->messageParts - 1);
	this->buffer = (BYTE*)malloc(this->bufferLength);
	if (!this->buffer) throw new std::exception("malloc fail");
		
	ULONG tmpItr = 0;

	// Action
	for (int i = 0; (i < this->actionLength) && this->action; i++)
	{
		this->buffer[tmpItr++] = this->action[i];
		if (tmpItr > this->bufferLength) throw new std::exception("writting out of range");
	}

	// Delim
	for (int i = 0; i < this->delimRepeat; i++)
	{
		this->buffer[tmpItr++] = this->delim;
		if (tmpItr > this->bufferLength) throw new std::exception("writting out of range");
	}

	// Sign
	for (int i = 0; (i < this->signLength) && this->sign; i++)
	{
		this->buffer[tmpItr++] = this->sign[i];
		if (tmpItr > this->bufferLength) throw new std::exception("writting out of range");
	}

	// Delim
	for (int i = 0; i < this->delimRepeat; i++)
	{
		this->buffer[tmpItr++] = this->delim;
		if (tmpItr > this->bufferLength) throw new std::exception("writting out of range");
	}

	// Content
	for (ULONG i = 0; (i < this->contentLength) && this->content; i++)
	{
		this->buffer[tmpItr++] = this->content[i];
		if (tmpItr > this->bufferLength) throw new std::exception("writting out of range");
	}
}

const BYTE* Message::getContent()
{
	return this->content;
}

int Message::getContentLength()
{
	return this->contentLength;
}

const char * Message::getAction()
{
	return this->action;
}

int Message::getActionLength()
{
	return this->actionLength;
}

const char * Message::getSign()
{
	return this->sign;
}

int Message::getSignLength()
{
	return this->signLength;
}
