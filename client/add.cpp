#include "add.h"
#include "ui_add.h"


add::add(QWidget *parent,QTcpSocket* m_client,QString m_user) :
    QWidget(parent),
    ui(new Ui::add)
{
    ui->setupUi(this);
    client=m_client;
    this->user_name=m_user;
    this->setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->cancel,&QPushButton::clicked,[=](){
        this->close();
    });

    connect(ui->ad,&QPushButton::clicked,[=](){
        QString user=ui->lineEdit->text();
        QJsonObject temp;
        temp.insert("type",6);
        temp.insert("request_user",this->user_name);
        temp.insert("requested",user);
        QJsonDocument re=QJsonDocument(temp);
        QByteArray str=re.toJson();
        //将消息发送出去
        int lenth=strlen(str);
        datahead header={lenth};
        this->client->write((const char*)&header,sizeof(datahead));
        client->write(str);
        QMessageBox::information(this,"提示","已发送请求");
        this->close();
    });
}

add::~add()
{
    delete ui;
}
