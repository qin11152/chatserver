#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include "regist.h"


namespace Ui {
class addfriend;
}

class addfriend : public QWidget
{
    Q_OBJECT

public:
    explicit addfriend(QWidget *parent = nullptr,QString user="root",QString request_name=" ",QTcpSocket *m_client=nullptr);
    ~addfriend();

private:
    Ui::addfriend *ui;
    QString username;
    QString request;
    QTcpSocket *client;
};

#endif // ADDFRIEND_H
