#pragma once
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>
using namespace std;


#define bufsize 10240


class tcpclient
{
public:

	tcpclient(SOCKET client = INVALID_SOCKET, int id = 0);
	virtual ~tcpclient();
	tcpclient(const tcpclient& client);
	int getid()const;
	SOCKET getclient()const;
	void setid(int my_id);
	char* returnbuf() { return _infobuf; }
	int returnposition() { return this->_lastposition; }
	void setposition(int now) { this->_lastposition = now; }
private:
	int m_id;
	SOCKET m_client;
	char _infobuf[bufsize*10] = {};
	int _lastposition = 0;
};

