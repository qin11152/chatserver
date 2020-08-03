#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QTcpSocket>

class tcpclient
{
public:
    tcpclient();
    bool connectserver();
    bool closeconnect();
    QTcpSocket* getsocket();
    ~tcpclient();
private:
    QTcpSocket* client=new QTcpSocket();
};

#endif // TCPCLIENT_H
