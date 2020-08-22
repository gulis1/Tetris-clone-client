#include "tSocket.h"

tSocket::tSocket(const string &address, const string &port) {
	
	this->adress = address.c_str();
	this->port = port.c_str();

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_CANONNAME | AI_ALL | AI_ADDRCONFIG;

	

}


bool tSocket::connectSocket(const char* address, const char* port) {

	int err = getaddrinfo(this->adress, this->port, &this->hints, &this->result);
	if (err != 0)
	{
		cout << "Invalid adress: " << err << endl;
		return false;
	}

	this->sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (sock == INVALID_SOCKET) {
		err = WSAGetLastError();
		cout << "An error cocurred creating the socket: " << err << endl;
		return false;
	}
	
	if (connect(sock, result->ai_addr, result->ai_addrlen)) {
		int err = WSAGetLastError();

		cout << "Could not connect: " << err << endl;
		return false;
	}


	return true;

}

const char* tSocket::getAdress() {
	return this->adress;
}

const char* tSocket::getPort() {
	return this->port;
}

string tSocket::sendMsg(const char* msg) {

	if (connectSocket(this->adress, this->port)) {
		send(sock, msg, strlen(msg), 0);

		char buffer[100] = { 0 };
		int valread = recv(this->sock, buffer, 100, 0);

		closesocket(this->sock);

		if (valread > 0)
			return buffer;

		else
			return "0";
	}


	return "0";
}



tSocket::~tSocket() {

	shutdown(this->sock, SD_SEND);
}