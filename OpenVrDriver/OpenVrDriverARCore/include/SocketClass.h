
#pragma once
#ifndef SocketClass_H
#define SocketClass_H

//#include <iostream>
#include <ws2tcpip.h>

class SocketClass
{

	SOCKET serverSocket, acceptSocket;
	int port;
	WSADATA wsaData;
	int wsaerr;
	WORD wVersionRequested;
public:
	bool GetStatus();

	void Connect(int port);

	void Receive(char* outStr);

	void CloseSocket();
};

#endif
