#include "tcpclient.h"
#include <QTcpSocket>
#include <QTcpServer>
#include <QtDebug>

tcpclient::tcpclient()
{
    qDebug()<<"tcpclient构造";
}

tcpclient::~tcpclient()
{
    delete client;
}



bool tcpclient::connectserver()
{
    QString ip="127.0.0.1";//
    qint16 port=809;
    client->connectToHost((QHostAddress)ip,port);
    qDebug()<<"连接成功";
    return true;
}

QTcpSocket* tcpclient::getsocket()
{
    return this->client;
}


bool tcpclient::closeconnect()
{
    qDebug()<<"tcpclient析构";
    client->close();
    return true;
}
