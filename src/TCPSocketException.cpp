#include "..\inc\TCPSocketException.h"

TCPSocketException::TCPSocketException(const char * msg)
{
	this->msg = msg;
}

TCPSocketException::TCPSocketException()
{
	this->msg = "TCP Socket error!";
}

const char * TCPSocketException::what() const throw()
{
	return this->msg;
}
