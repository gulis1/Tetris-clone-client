#pragma once
#include<iostream>
#include<string>
#include<WinSock2.h>
#include <Ws2tcpip.h>

using namespace std;

class tSocket
{
	private:
		addrinfo hints;
		addrinfo* result = NULL;
		int sock;
		const char* adress;
		const char* port;
		bool connectSocket(const char* addres, const char* port);
	
	public:
		const char* getAdress();
		const char* getPort();
		tSocket(const string &address, const string & port);
		string sendMsg(const char* msg);
		~tSocket();
};

