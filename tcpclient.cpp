#include "tcpclient.h"
#include "tcpclient.h"
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <string>




#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"libmysql.lib")



tcpclient::tcpclient(SOCKET client, int id)
{
	m_client = client;
	m_id = id;
}


tcpclient::tcpclient(const tcpclient& client)
{
	this->m_client = client.m_client;

	this->m_id = client.m_id;
}


tcpclient::~tcpclient()
{
	closesocket(m_client);
}

int tcpclient::getid()const
{
	return m_id;
}

SOCKET tcpclient::getclient() const
{
	return m_client;
}

void tcpclient::setid(int my_id)
{
	this->m_id = my_id;
}

