#pragma once

typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned char BYTE;

class Message
{
public:
	Message(const char* type, const char* sign, const BYTE* content, UINT contentLength);
	Message(const BYTE* message, ULONG length);
	Message();
	void from(const BYTE* message, ULONG length);
	void to(const char* type, const char* sign, const BYTE* content, UINT contentLength);
	const BYTE* getBuffer();
	int getBufferLength();
	void setContent(const BYTE* content, UINT contentLength);
	const BYTE* getContent();
	int getContentLength();
	void setAction(const char* type);
	const char* getAction();
	int getActionLength();
	void setSign(const char* sign);
	const char* getSign();
	int getSignLength();

private:
	void updateBuffer();

private:
	static constexpr const char delim				= 0;
	static constexpr const int delimRepeat			= 1;
	static constexpr const int messageParts			= 3;

	char* action;
	int actionLength;
	BYTE* content;
	ULONG contentLength;
	BYTE* buffer;
	ULONG bufferLength;
	char* sign;
	int signLength;
};

