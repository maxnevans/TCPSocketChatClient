#pragma once
#include <exception>
using std::exception;

class TCPSocketException :
	public exception
{
public:
	TCPSocketException(const char* msg);
	TCPSocketException();
	const char* what() const throw ();

private:
	const char* msg;
};

