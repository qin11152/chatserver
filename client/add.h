#ifndef ADD_H
#define ADD_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpSocket>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include "regist.h"


namespace Ui {
class add;
}

class add : public QWidget
{
    Q_OBJECT

public:
    explicit add(QWidget *parent = nullptr,QTcpSocket* m_client=nullptr,QString m_user="");
    ~add();

private:
    Ui::add *ui;
    QTcpSocket *client;
    QString user_name;
};

#endif // ADD_H
