#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include "sig.h"
#include <QDebug>
#include <QLabel>
#include <QDateTime>
#include <QSharedPointer>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTextCodec>
#include <QString>
#include <synchapi.h>
#include "regist.h"



namespace Ui {
class chat;
}

class chat : public QWidget
{
    Q_OBJECT

public:
    explicit chat(QWidget *parent = nullptr,sig* m_si=nullptr,QString m_chatuser="",QTcpSocket* m_client=nullptr,QString m_name="");
    ~chat();
    bool updatechat(QString info);
    bool send(QString message);
    bool getrecord();

public slots:
    void handle(QString name,QString info);
    void recvhandle(QString name);

private:
    Ui::chat *ui;
    sig* si;
    QTcpSocket* client;
    QString chatuser;
    QString my_name;
    //QVector<QSharedPointer<QWidget>> widgetmap;
    //QVector<QSharedPointer<QListWidgetItem>> itemmap;
    QVector<QWidget*> widgetmap;
    QVector<QListWidgetItem*> itemmap;
};

#endif // CHAT_H
