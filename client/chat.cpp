#include "chat.h"
#include "ui_chat.h"
typedef enum informationtype {
    Register,
    Login,
    Chat,
    Logout,
    Init,
    Getlist,
    Addfriend,
    AgreeAdd,
    InitRequest,
    GetRecord,
} infoname;

//QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
chat::chat(QWidget *parent,sig* m_si,QString m_chatuser,QTcpSocket* m_client,QString m_name) :
    QWidget(parent),
    ui(new Ui::chat)
{
    ui->setupUi(this);
    this->si=m_si;
    chatuser=m_chatuser;
    client=m_client;
    my_name=m_name;
    ui->show->scrollToBottom();
    this->setAttribute(Qt::WA_DeleteOnClose);
    getrecord();
    connect(ui->send,&QPushButton::clicked,[=](){
        QString temp=ui->infor->toPlainText();
        qDebug()<<temp;
        char buffer[1024]={0};
        ui->infor->clear();
        for(int i=0;i<temp.length();++i)
        {
            QChar t=temp.at(i);
            buffer[i]=t.toLatin1();
        }
        //qDebug()<<buffer;
        QString message=buffer;
        QByteArray ta=message.toUtf8();
        QString temp1=QTextCodec::codecForName("GB2312")->toUnicode(ta);
        //qDebug()<<"anle:"<<temp1;
        send(temp);
        Sleep(100);
        getrecord();
    });
}

void chat::handle(QString name,QString info)
{
    if(name==this->chatuser)
    {
        qDebug()<<"?";
        updatechat(info);
    }
}

bool chat::updatechat(QString info)
{
    QTextCodec* code=QTextCodec::codecForName("GB2312");
    for(int i=0;i<widgetmap.size();i++)
    {
        delete widgetmap[i];
    }
    widgetmap.clear();
    for(int i=0;i<itemmap.size();++i)
    {
        delete itemmap[i];
    }
    itemmap.clear();
     QByteArray me=info.toUtf8().data();
     QJsonParseError json_error;
     QJsonDocument json_message=QJsonDocument::fromJson(me, &json_error);
     if(json_error.error==QJsonParseError::NoError&&!json_message.isNull())
     {
         //qDebug()<<json_message;
     }
     QJsonObject json_object=json_message.object();
     //QString str=json_object.value("messa0").toString();
     //qDebug()<<"utf8"<<str;
     int nums=json_object.value("count").toInt();
     qDebug()<<"nums: "<<nums;
     QVector<QString> messages(nums);
     QVector<QString> times(nums);
      QVector<QString> fromwheres(nums);
     for(int i=0;i<nums;i++)
     {
        QString temp1= "messa"+QString::number(i);
        QString tr1=json_object.value(temp1).toString();
        qDebug()<<"原始："<<tr1;
        QByteArray tr2=tr1.toUtf8();
        qDebug()<<"tr2: "<<tr2;
        QString tr3=code->toUnicode(tr2);
        qDebug()<<"变换："<<tr3;
        messages[nums-i-1]=tr1;
        temp1="time"+QString::number(i);
        times[nums-i-1]=(json_object.value(temp1).toString());
        temp1="fromwhere"+QString::number(i);
        fromwheres[nums-i-1]=(json_object.value(temp1).toString());
     }
    //qDebug()<<info;
    //QByteArray ta=info.toUtf8();
    //QString temp=QTextCodec::codecForName("GB2312")->toUnicode(ta);
    //qDebug()<<temp;
    for(int i=0;i<nums;i++)
    {
        qDebug()<<"建造";
        QWidget *te=new QWidget(this);
        widgetmap.push_back(te);
        QLabel *la=new QLabel(te);
        te->setMaximumWidth(ui->show->width()-50);
        te->setStyleSheet("QWidget{border:1px;"//边框宽度
                          "border-style:solid;"//边框风格
                          "border-color:black"//边框颜色
                          "background:white}");//背景颜色
        la->setText(messages[i]);
        qDebug()<<messages[i];
        la->adjustSize();
        la->setWordWrap(true);
        la->setAlignment(Qt::AlignTop);
        la->resize(te->size());
        QListWidgetItem *item=new QListWidgetItem();
        QListWidgetItem *txtitem= new QListWidgetItem();
        itemmap.push_back(item);
        itemmap.push_back(txtitem);
        item->setText(fromwheres[i]+" "+times[i]);
        item->setFont(QFont("宋体",7));
        la->setFont(QFont("楷书",12));
        ui->show->addItem(item);
        ui->show->addItem(txtitem);
        ui->show->setItemWidget(txtitem,te);
        txtitem->setSizeHint(QSize(0,la->rect().height()));
    }
    return true;
}

bool chat::send(QString message)
{
    QJsonObject chat;
    //QString infom=QString::fromLocal8Bit(message.toUtf8());
    QString infom;
    QTextCodec* code=QTextCodec::codecForName("UTF-8");
    QByteArray temp=message.toUtf8();
    infom=code->toUnicode(temp);
    qDebug()<<"send: "<<infom;
    chat.insert("type",Chat);
    chat.insert("fromwhere",this->my_name);
    chat.insert("towhere",this->chatuser);
    chat.insert("message",infom);
    QJsonDocument ree=QJsonDocument(chat);
    QByteArray str=ree.toJson();
    int lenth=strlen(str);
    datahead header={lenth};
    this->client->write((const char*)&header,sizeof(datahead));
    this->client->write(str);
    return true;
}

bool chat::getrecord()
{
    QJsonObject temp;
    temp.insert("type",9);
    temp.insert("username",this->my_name);
    temp.insert("chatuser",this->chatuser);
    QJsonDocument re=QJsonDocument(temp);
    QByteArray str=re.toJson();
    //发送消息
    int lenth=strlen(str);
    datahead header={lenth};
    this->client->write((const char*)&header,sizeof(datahead));
    client->write(str);
    return true;
}

void chat::recvhandle(QString name)
{
    if(name==this->chatuser)
    {
        getrecord();
    }
}

chat::~chat()
{
    emit this->si->guanbi(this->chatuser);
    itemmap.clear();
    widgetmap.clear();
    delete ui;
}
