#ifndef MAINLIST_H
#define MAINLIST_H

#include <QWidget>
#include <QSharedPointer>
#include "tcpclient.h"
#include <QListWidget>
#include <QHash>
#include "sig.h"
#include <QJsonDocument>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QDebug>
#include <QPushButton>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonValue>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHash>
#include <windows.h>
#include <addfriend.h>
#include "add.h"
#include "sig.h"
#include "chat.h"
#include <QCloseEvent>
#include <QMessageBox>
#include "regist.h"

namespace Ui {
class mainlist;
}

class mainlist : public QWidget
{
    Q_OBJECT

public:
    explicit mainlist(QString m_username="1",QWidget *parent = nullptr);
    ~mainlist();

    //bool initconnect(QTcpSocket* client);
    bool initconnect();
    bool addmember(const QString username);
    QListWidgetItem* insertItem(int row,const QString username);
    bool updatelist(QVector<QString> friends,QListWidget* list,QTcpSocket* client);
    bool getlist(QTcpSocket* client);
    bool initrequest(QTcpSocket* client);
    bool connectserver();
    void logout();
    void closeEvent(QCloseEvent *event);

signals:

public slots:
    void guan(QString name);

private:
    Ui::mainlist *ui;
    QString username;
    QVector<QWidget*> friendlistsss;
    QTcpSocket* client;
    //QVector<QSharedPointer<QWidget>> friendlistsss;
    QHash<QString,int> isopen;
    sig* si;
    //QVector<QSharedPointer<QListWidgetItem>> itemmap;
    QVector<QListWidgetItem*> itemmap;
    QVector<QPushButton*> btnmap;
};

#endif // MAINLIST_H
