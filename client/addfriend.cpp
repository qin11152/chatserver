#include "addfriend.h"
#include "ui_addfriend.h"



addfriend::addfriend(QWidget *parent,QString user,QString request_name,QTcpSocket* m_client) :
    QWidget(parent),
    ui(new Ui::addfriend)
{
    username=user;
    client=m_client;
    request=request_name;
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    QString str=request_name+"请求添加你为好友，是否同意？";
    ui->info->setText(str);

    connect(ui->agree,&QPushButton::clicked,[=](){
        QJsonObject temp;
        temp.insert("type",7);
        temp.insert("request_user",this->request);
        temp.insert("requested",this->username);
        QJsonDocument re=QJsonDocument(temp);
        QByteArray str=re.toJson();
        //将消息发送出去
        int lenth=strlen(str);
        datahead header={lenth};
        this->client->write((const char*)&header,sizeof(datahead));
        client->write(str);
        QMessageBox::information(this,"提示","已同意请求");
        this->close();
    });


    connect(ui->disagree,&QPushButton::clicked,[=](){
        QMessageBox::information(this,"提示","已拒绝请求");
        this->close();
    });
}

addfriend::~addfriend()
{
    delete ui;
}
