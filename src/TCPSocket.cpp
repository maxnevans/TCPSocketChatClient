#include "..\inc\TCPSocket.h"
#include "..\inc\TCPSocketException.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

typedef TCPSocketException Exception;


TCPSocket::TCPSocket(const char* remoteAddr, const char* remotePort)
{
	WSADATA wsaData;
	int iRetVal;

	iRetVal = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iRetVal != ERROR_SUCCESS)
	{
		char buffer[256];
		sprintf_s(buffer, "WSAStartup fail : %d\n", iRetVal);
		throw new Exception(buffer);
	}

	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iRetVal = getaddrinfo(remoteAddr, remotePort, &hints, &result);
	if (iRetVal != ERROR_SUCCESS)
	{
		char buffer[256];
		sprintf_s(buffer, "getaddrinfo fail : %d\n", iRetVal);
		throw new Exception(buffer);
	}

	this->ConnectSocket = INVALID_SOCKET;
	ptr = result;
	this->ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (this->ConnectSocket == INVALID_SOCKET)
	{
		char buffer[256];
		sprintf_s(buffer, "socket fail : %ld\n", WSAGetLastError());
		throw new Exception(buffer);
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{

		this->ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (this->ConnectSocket == INVALID_SOCKET)
		{
			WSACleanup();
			char buffer[256];
			sprintf_s(buffer, "socket failed with error: %ld\n", WSAGetLastError());
			throw new Exception(buffer);
		}

		iRetVal = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iRetVal == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}


	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		char buffer[256];
		sprintf_s(buffer, "Unable to connect to the server!\n");
		throw new Exception(buffer);
		WSACleanup();
	}

}

void TCPSocket::sendData(const BYTE* data, int length)
{
	int iRetVal;
	iRetVal = send(this->ConnectSocket, (char*)data, length, 0);
	if (iRetVal == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		WSACleanup();
		char buffer[256];
		sprintf_s(buffer, "send failed with error: %d\n", WSAGetLastError());
		throw new Exception(buffer);
		
	}
	else if (iRetVal != length) throw new std::exception("socket send failed: sent not the whole message");
}

int TCPSocket::recvData(BYTE* bufer, int bufferLength)
{
	int iRetVal;
	iRetVal = recv(this->ConnectSocket, (char*)bufer, bufferLength, 0);
	if (iRetVal < 0)
	{
		closesocket(ConnectSocket);
		WSACleanup();
		char buffer[256];
		sprintf_s(buffer, "recv failed: %d\n", WSAGetLastError());
		throw new Exception(buffer);
	}
	return iRetVal;
}

TCPSocket::~TCPSocket()
{
	closesocket(this->ConnectSocket);
	WSACleanup();
}
