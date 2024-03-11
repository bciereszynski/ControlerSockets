#pragma once

#include "Socket.h"
#include <exception>
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <cstdio>

class SocketManager {

protected:
	SocketManager();
	sockaddr_in CreateService(const char* addr, u_short port);

public:
	virtual SmartSocket* CreateSocket(const char* addr, u_short port) = 0;
	virtual inline ~SocketManager() {
		WSACleanup();
	}
};

class TCPSocketManager : public SocketManager {
	void Connect(SOCKET* sc, sockaddr_in* service);
public:
	TCPSocketManager() : SocketManager() {}
	TCPSocket* CreateSocket(const char* addr, u_short port) override;
};

class UDPSocketManager : public SocketManager {
public:
	UDPSocketManager() : SocketManager() {}
	SmartSocket* CreateSocket(const char* addr, u_short port) override;
};