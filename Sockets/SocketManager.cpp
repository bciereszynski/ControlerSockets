#include <iostream>
#include "SocketManager.h"

SocketManager::SocketManager() {
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
		std::cerr << "Initialization error...\n";
	}

sockaddr_in SocketManager::CreateService(const char* addr, u_short port) {
	sockaddr_in service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(addr);
	service.sin_port = htons(port);
	return service;
}


void TCPSocketManager::Connect(SOCKET* sc, sockaddr_in* service) {
	if (connect(*sc, (SOCKADDR*)service, sizeof(*service)) == SOCKET_ERROR)
	{
		std::cerr << "Failed to connect to remote socket..."  << WSAGetLastError() << "\n";
		WSACleanup();
		throw std::exception("connect");
	}
}

TCPSocket* TCPSocketManager::CreateSocket(const char* addr, u_short port)
{
	SOCKET sc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sc == INVALID_SOCKET)
	{
		std::cerr << "Error creating socket..." << WSAGetLastError() << "\n";
		WSACleanup();
		throw std::exception("create socket error");
	}
	sockaddr_in service = CreateService(addr, port);
	Connect(&sc, &service);
	return new TCPSocket(sc);
}


SmartSocket* UDPSocketManager::CreateSocket(const char* addr, u_short port)
{
	SOCKET sc = socket(AF_INET, SOCK_DGRAM, 0);
	if (sc == INVALID_SOCKET)
	{
		std::cerr << "Error creating socket..." << WSAGetLastError() << "\n";
		WSACleanup();
		throw std::exception("create socket error");
	}
	sockaddr_in service = CreateService(addr, port);
	return new UDPSocket(sc, service);
}