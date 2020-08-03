#include "regist.h"


regist::regist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::regist)
{
    ui->setupUi(this);

    //建立TP连接

    tcpclient* tclient=new tcpclient();
    tclient->connectserver();
    QTcpSocket* client=tclient->getsocket();


    connect(ui->finish,&QPushButton::clicked,[=](){
        QString user=ui->user->text();
        QString passwd1=ui->passwd->text();
        QString passwd2=ui->passwd1->text();
        if(passwd1!=passwd2)
        {
            QMessageBox::critical(this,"错误","两次输入密码不相同");
        }else
        {
            QJsonObject temp;   //建立json报文
            temp.insert("type",0);
            temp.insert("username",user);
            temp.insert("passwd",passwd1);
            QJsonDocument messa=QJsonDocument(temp);
            QByteArray str=messa.toJson();

            //建立TP连接
            tcpclient* tclient=new tcpclient();
            tclient->connectserver();
            QTcpSocket* client=tclient->getsocket();
            int lenth=strlen(str);
            datahead header={lenth};
            client->write((const char*)&header,sizeof(datahead));
            client->write(str);
            //接受服务器传回消息

            connect(client,&QTcpSocket::readyRead,[=](){
                QString message=client->readAll();
                if(message=="yes")
                {
                    QMessageBox::information(this,"提示","注册成功");
                    client->disconnect();
                }
                if(message=="error")
                {
                    QMessageBox::critical(this,"注册失败","请重试");
                    client->disconnect();
                }
                if(message=="user already exist")
                {
                    QMessageBox::critical(this,"提示","用户已存在");
                    client->disconnect();
                }
            });
        }



    });
}

regist::~regist()
{
    delete ui;
}
