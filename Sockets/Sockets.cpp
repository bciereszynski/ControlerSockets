#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <thread>

#include <Ws2tcpip.h>
#include <winsock2.h>

#pragma comment(lib,"WS2_32")

class SmartSocket {
public:
	virtual int write(const char* sendbuf) = 0;
	virtual int read(char* recvbuf, int len) = 0;
};

class TCPSocket : public SmartSocket {
private:
	SOCKET sc;
public:
	TCPSocket(SOCKET _sc) {
		sc = _sc;
	}
	int write(const char* sendbuf) {
		return send(sc, sendbuf, strlen(sendbuf), 0);
	}
	int read(char* recvbuf, int len) {
		return recv(sc, recvbuf, len, 0);
	}
};

class UDPSocket : public SmartSocket {
private:
	sockaddr_in servaddr;
	SOCKET sc;
public:
	UDPSocket(SOCKET _sc, sockaddr_in _servaddr) {
		sc = _sc;
		servaddr = _servaddr;
	}
	int write(const char* sendbuf) {
		return sendto(sc, (const char*)sendbuf, strlen(sendbuf),
			0, (const struct sockaddr*)&servaddr,
			sizeof(servaddr));

	}

	int read(char* recvbuf, int len) {
		socklen_t leng;
		int n = recvfrom(sc, (char*)recvbuf, len,
			MSG_WAITALL, (struct sockaddr*)&servaddr,
			&leng);
		recvbuf[n] = '\0';
		return n;
	}
};

class SocketManager {

protected:
	SocketManager() {
		WSADATA wsaData;

		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != NO_ERROR)
			printf("Initialization error.\n");
	}

	sockaddr_in CreateService(const char* addr, u_short port) {
		sockaddr_in service;
		memset(&service, 0, sizeof(service));
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = inet_addr(addr);
		service.sin_port = htons(port);
		return service;
	}

public:
	virtual SmartSocket* CreateSocket(const char* addr, u_short port) = 0;
	virtual ~SocketManager() {
		WSACleanup();
	}
};

class TCPSocketManager : public SocketManager {
	void Connect(SOCKET* sc, sockaddr_in* service) {
		if (connect(*sc, (SOCKADDR*)service, sizeof(*service)) == SOCKET_ERROR)
		{
			printf("connect: %d\n", WSAGetLastError());
			printf("Failed to connect.\n");
			WSACleanup();
		}
	}
public:
	TCPSocketManager() : SocketManager() {}
	TCPSocket* CreateSocket(const char* addr, u_short port) override
	{
		SOCKET sc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sc == INVALID_SOCKET)
		{
			printf("Error creating socket: %ld\n", WSAGetLastError());
			WSACleanup();
			throw std::exception("create socket error");
		}
		sockaddr_in service = CreateService(addr, port);
		Connect(&sc, &service);
		return new TCPSocket(sc);
	}
};

class UDPSocketManager : public SocketManager {
public:
	UDPSocketManager() : SocketManager() {}
	SmartSocket* CreateSocket(const char* addr, u_short port) override
	{
		SOCKET sc = socket(AF_INET, SOCK_DGRAM, 0);
		if (sc == INVALID_SOCKET)
		{
			printf("Error creating socket: %ld\n", WSAGetLastError());
			WSACleanup();
			throw std::exception("create socket error");
		}
		sockaddr_in service = CreateService(addr, port);
		return new UDPSocket(sc, service);
	}
};

int main()
{
	SocketManager* sm = new TCPSocketManager();

	TCPSocket* sc = (TCPSocket*)sm->CreateSocket("127.0.0.1", 8080);

	int bytesSent;
	int bytesRecv = SOCKET_ERROR;
	char sendbuf[32] = "HUJ";
	char recvbuf[32] = "";

	bytesSent = (*sc).write(sendbuf);
	printf("Bytes sent: %ld\n", bytesSent);

	while (bytesRecv == SOCKET_ERROR)
	{
		bytesRecv = (*sc).read(recvbuf, 32);

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

	system("pause");

}

