#include "SocketClass.h"


	SOCKET serverSocket, acceptSocket;
	int port;
	WSADATA wsaData;
	int wsaerr;
	WORD wVersionRequested;
	bool running;

	void SocketClass::Connect(int port = 55455) {
		this->port = port;
		wVersionRequested = MAKEWORD(2, 2); //sets version of winsock
		wsaerr = WSAStartup(wVersionRequested, &wsaData);
		if (wsaerr != 0) {
			//vr::VRDriverLog()->Log("Winsock dll not found!");
			return;
		}
		else {
		//	vr::VRDriverLog()->Log("Winsock dll was found!");
		}
		serverSocket = INVALID_SOCKET;
		serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (serverSocket == INVALID_SOCKET) {
			//vr::VRDriverLog()->Log("Error at socket():" + WSAGetLastError());
			WSACleanup();
			return;
		}
		else {
			//vr::VRDriverLog()->Log("socket() is ok!");
		}

		sockaddr_in service;
		service.sin_family = AF_INET;
		InetPton(AF_INET, L"0.0.0.0", &service.sin_addr.s_addr);
		service.sin_port = htons(port);
		//service.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		//	vr::VRDriverLog()->Log("bind() failed" + WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return;
		}
		else {
			//vr::VRDriverLog()->Log("bind() ok");
			running = true;
		}


	}

	bool SocketClass::GetStatus() {
		return running;
	}

	void SocketClass::Receive(char* outStr) {

		DWORD RecvCount = 0;


		struct sockaddr_in si_other;
		int slen = sizeof(si_other);
		char buffer[2048];
		//int byteCount = recv(serverSocket, buffer, 2048, 0);
		int byteCount = recvfrom(serverSocket, buffer, 2048, 0, (struct sockaddr*)&si_other, &slen);

		if (byteCount > 0) {
			for (int i = 0;i < byteCount;i++) {
				outStr[i] = buffer[i];
			}
		}
		else {
			//	vr::VRDriverLog()->Log("Message Failed");
			WSACleanup();
			outStr = NULL;
		}
	}

	void SocketClass::CloseSocket() {
		running = false;
		closesocket(serverSocket);
		WSACleanup();
	}


