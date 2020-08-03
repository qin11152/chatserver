
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    connect(ui->test,&QPushButton::clicked,[=](){
        mainlist *list=new mainlist();
        list->setAttribute(Qt::WA_DeleteOnClose);
        list->show();
    });


    //client=new QTcpSocket(this);
    connect(ui->signin,&QPushButton::clicked,[=](){

        //获取文本框内容
        QString user=ui->user->text();
        QString passwd=ui->passwd->text();

        //建立tcp连接，短连接
        tcpclient* tclient=new tcpclient();
        //tcpclient* tclient;
        tclient->connectserver();
        QTcpSocket* client=tclient->getsocket();

        //封装为json格式

        QJsonObject temp;
        temp.insert("type",1);
        temp.insert("username",user);
        temp.insert("passwd",passwd);
        QJsonDocument re=QJsonDocument(temp);
        QByteArray str=re.toJson();
        int lenth=qstrlen(str);
        //lenth++;
        datahead header={lenth};
        /*char* st={};
        memcpy(st,header,sizeof(datahead));
        QByteArray str1=st;*/
        qDebug()<<lenth;

        client->write((char*)&header,sizeof(datahead));
        //发送信息
        client->write(str);

        //接受服务器传回消息

        connect(client,&QTcpSocket::readyRead,[=](){
        QString s=client->readAll();
        qDebug()<<s;

        //处理传回的信息


        if(s=="error")
        {
            QMessageBox::critical(this,"提示","密码错误");
            tclient->closeconnect();
        }
        if(s=="alreadylogin")
        {
            QMessageBox::critical(this,"提示","用户已登录");
            tclient->closeconnect();
        }
        if(s=="yes")
        {
            //QMessageBox::information(this,"提示","登陆成功");
            tclient->closeconnect();
            mainlist* list=new mainlist(user);
            list->setAttribute(Qt::WA_DeleteOnClose);
            list->show();
            this->close();
        }
        if(s=="user doesn't exist")
        {
            QMessageBox::critical(this,"错误","用户不存在");
            tclient->closeconnect();
        }
        });

    });


    connect(ui->regist,&QPushButton::clicked,[=](){
        regist *regis=new regist();
        regis->setAttribute(Qt::WA_DeleteOnClose);
        regis->show();
    });


}

MainWindow::~MainWindow()
{
    delete ui;
}




/*bool MainWindow::connectserver()
{
    QString ip="127.0.0.1";
    qint16 port=809;
    client->connectToHost((QHostAddress)ip,port);
    return true;
}*/
