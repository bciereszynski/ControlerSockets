#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#include <Ws2tcpip.h>
#include <winsock2.h>
#include <cstdio>

class SmartSocket {
public:
	virtual int write(const char* sendbuf) = 0;
	virtual int read(char* recvbuf, int len) = 0;
};

class TCPSocket : public SmartSocket {
private:
	SOCKET sc;
public:
	TCPSocket(SOCKET _sc);
	int write(const char* sendbuf) override;
	int read(char* recvbuf, int len) override;
	~TCPSocket();
};


class UDPSocket : public SmartSocket {
private:
	sockaddr_in servaddr;
	SOCKET sc;
public:
	UDPSocket(SOCKET _sc, sockaddr_in _servaddr);
	int write(const char* sendbuf) override;
	int read(char* recvbuf, int len) override;
	~UDPSocket();
};