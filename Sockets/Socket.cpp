#include "Socket.h"



TCPSocket::TCPSocket(SOCKET _sc) {
	sc = _sc;
}
int TCPSocket::write(const char* sendbuf, size_t len) {
	return send(sc, sendbuf, len, 0);
}
int TCPSocket::read(char* recvbuf, int len) {
	return recv(sc, recvbuf, len, 0);
}
TCPSocket::~TCPSocket() {
	closesocket(sc);
}


UDPSocket::UDPSocket(SOCKET _sc, sockaddr_in _servaddr) {
	sc = _sc;
	servaddr = _servaddr;
}
int UDPSocket::write(const char* sendbuf, size_t len) {
	return sendto(sc, (const char*)sendbuf, len,
		0, (const struct sockaddr*)&servaddr,
		sizeof(servaddr));
}

int UDPSocket::read(char* recvbuf, int len) {
	int SenderAddrSize = sizeof(servaddr);
	int n = recvfrom(sc, (char*)recvbuf, len,
		MSG_WAITALL, (struct sockaddr*)&servaddr, &SenderAddrSize);
	return n;
}
UDPSocket::~UDPSocket() {
	closesocket(sc);
}
