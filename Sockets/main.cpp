#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <thread>

#include <Ws2tcpip.h>
#include <winsock2.h>
#include "Socket.h"
#include "SocketManager.h"

#pragma comment(lib,"WS2_32")


int main()
{
	SocketManager* sm = new UDPSocketManager();

	SmartSocket* sc = sm->CreateSocket("127.0.0.1", 8080);

	int bytesSent;
	int bytesRecv = SOCKET_ERROR;
	char sendbuf[32] = "HUJ";
	char recvbuf[32] = "";

	bytesSent = (*sc).write(sendbuf);
	printf("Bytes sent: %ld\n", bytesSent);

	while (bytesRecv == SOCKET_ERROR)
	{
		bytesRecv = (*sc).read(recvbuf, 30);

		if (bytesRecv == 0 || bytesRecv == WSAECONNRESET)
		{
			printf("Connection closed.\n");
			break;
		}

		if (bytesRecv < 0)
			return 1;

		printf("Bytes received: %ld\n", bytesRecv);
		printf("Received text: %s\n", recvbuf);
	}

}

