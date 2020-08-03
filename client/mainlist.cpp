#include "mainlist.h"
#include "ui_mainlist.h"
#include "tcpclient.h"




#define arrow_size 20
#define head_size 40;


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


mainlist::mainlist(QString m_username,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainlist),
    username(m_username)
{
    ui->setupUi(this);
    qDebug()<<"this user: "<<username;
    //建立tcp连接，长连接
    //tcpclient* link=new tcpclient();
    //link->connectserver();
    this->client=new QTcpSocket();
    connectserver();
    //初始化与server连接
    initconnect();
    getlist(client);

    //建立列表
    QListWidget* mainlists=new QListWidget(this);
    mainlists->show();

    //initrequest(client);

    this->si=new sig();

    connect(client,&QTcpSocket::readyRead,[=](){
            QString s=client->readAll();
            /*QByteArray ty=s.toUtf8();
            const char* buffer=ty.data();
            memcpy(_recvbuf+_lastposi,buffer,s.length());*/
            qDebug()<<s;
            QByteArray message=s.toUtf8().data();
            QJsonParseError json_error;
            QJsonDocument json_message=QJsonDocument::fromJson(message, &json_error);
            if(json_error.error==QJsonParseError::NoError&&!json_message.isNull())
            {
                //qDebug()<<json_message;
            }
            QJsonObject json_object=json_message.object();
            int m_type=json_object.value("type").toInt();
            qDebug()<<m_type;
            switch(m_type)
            {
            case Getlist:
            {
                QVector<QString> m_list;
                int m_count=json_object.value("count").toInt();
                for(int i=0;i<m_count;i++)
                {
                    QString haoba="name"+QString::number(i);
                    m_list.push_back(json_object.value(haoba).toString());
                    updatelist(m_list,mainlists,client);
                }
            }
                break;
            case Addfriend:
            {
                QString requested=json_object.value("requested").toString();
                QString request_user=json_object.value("request_user").toString();
                addfriend* add=new addfriend(nullptr,requested,request_user,client);
                add->show();
            }
                break;
            case InitRequest:
            {
                int m_count=json_object.value("count").toInt();
                QVector<QString> request;
                for(int i=0;i<m_count;i++)
                {
                    QString temp="request"+QString::number(i);
                    QString request_user=json_object.value(temp).toString();
                    request.push_back(request_user);
                }
                for(int i=0;i<request.size();i++)
                {
                    QString requested=this->username;
                    QString request_user=request[i];
                    addfriend* add=new addfriend(nullptr,requested,request_user,client);
                    add->show();
                }
            }
                break;
            case GetRecord:
            {
                 QString chatuser=json_object.value("chatuser").toString();
                 qDebug()<<"chatuser"<<chatuser;
                 QString info=json_object.value("message").toString().toLocal8Bit();
                 qDebug()<<"传进去的："<<s;
                 emit si->chufa(chatuser,s);
            }
                break;
            case Chat:
            {
                QString user=json_object.value("fromwhere").toString();
                if(isopen[user]==1)
                {
                    emit si->infor(user);
                }else
                {
                    for(int i=0;i<btnmap.size();i++)
                    {
                        qDebug()<<"在找头像";
                        if(btnmap[i]->text()==user)
                        {
                             QIcon icon1(":/login/prefix1/image/qq1.jpg");
                             qDebug()<<"换头像了";
                             btnmap[i]->setIcon(icon1);
                             break;
                        }
                    }
                }
                QString messageinfo=json_object.value("message").toString().toUtf8();
                qDebug()<<"message: "<<messageinfo;
            }
                break;
            default:
                qDebug()<<"listen错误";
                break;
            }
        });

    QVector<QString> test;
    //test.push_back("root");
    updatelist(test,mainlists,client);

    connect(ui->addrequest,&QPushButton::clicked,[=](){
        initrequest(client);
    });

    connect(ui->addfriend,&QPushButton::clicked,[=](){
        add * ad=new add(nullptr,client,this->username);
        ad->show();
    });


    //获取好友列表
    connect(ui->refresh,&QPushButton::clicked,[=](){
        //向服务器发送消息
        /*QJsonObject temp;
        temp.insert("type",5);
        temp.insert("username",this->username);
        QJsonDocument re=QJsonDocument(temp);
        QByteArray str=re.toJson();
        client->write(str);*/
        getlist(client);
    });

    //发送消息

}
bool mainlist::connectserver()
{
    QString ip="127.0.0.1";//
    qint16 port=809;
    this->client->connectToHost((QHostAddress)ip,port);
    qDebug()<<"连接成功";
    return true;
}

void mainlist::closeEvent(QCloseEvent *event)
{
    QJsonObject chat;
    chat.insert("type",Logout);
    chat.insert("username",this->username);
    QJsonDocument ree=QJsonDocument(chat);
    QByteArray ch=ree.toJson();
    int lenth=strlen(ch);
    datahead header={lenth};
    this->client->write((const char*)&header,sizeof(datahead));
    this->client->write(ch);
    qDebug()<<" 按了结束";
    Sleep(100);
    event->accept();
}

bool mainlist::getlist(QTcpSocket *client)
{
    QJsonObject temp;
    temp.insert("type",5);
    temp.insert("username",this->username);
    QJsonDocument re=QJsonDocument(temp);
    QByteArray str=re.toJson();
    int lenth=strlen(str);
    qDebug()<<"getlist长度:"<<lenth;
    datahead header={lenth};
    this->client->write((const char*)&header,sizeof(datahead));
    client->write(str);
    return true;
}

bool mainlist::initconnect()
{
    //封装初始化消息
    qDebug()<<"初始化\n";
    QJsonObject temp;
    temp.insert("type",4);
    temp.insert("username",this->username);
    temp.insert("to",1);
    QJsonDocument re=QJsonDocument(temp);
    QByteArray str=re.toJson();
    //发送消息
    int lenth=strlen(str);
    qDebug()<<"init长度:"<<lenth;
    datahead header={lenth};
    this->client->write((const char*)&header,sizeof(datahead));
    this->client->write(str);
    return true;
}

bool mainlist::initrequest(QTcpSocket *client)
{
    qDebug()<<"初始化\n";
    QJsonObject temp;
    temp.insert("type",8);
    temp.insert("username","1052080761");
    QJsonDocument re=QJsonDocument(temp);
    QByteArray str=re.toJson();
    //发送消息
    int lenth=strlen(str);
    datahead header={lenth};
    this->client->write((const char*)&header,sizeof(datahead));
    client->write(str);
    return true;
}

bool mainlist::updatelist(QVector<QString> friends,QListWidget* list,QTcpSocket* client)
{
    //list->clear();
    //itemmap.clear();
    this->isopen.clear();
    for(int i=0;i<friendlistsss.size();i++)
    {
        delete friendlistsss[i];
    }
    friendlistsss.clear();
    for(int i=0;i<itemmap.size();i++)
    {
        delete itemmap[i];
    }
    itemmap.clear();
    for(int i=0;i<itemmap.size();i++)
    {
        delete btnmap[i];
    }
    btnmap.clear();
    for(int i=0;i<friends.size();i++)
    {
        isopen.insert(friends[i],0);
        QWidget *te=new QWidget(this);
        QPushButton* btn=new QPushButton(te);
        btn->setFixedSize(220,40);
        QIcon icon1(":/login/prefix1/image/qq.jpg");
        btn->setIcon(icon1);
        btn->setText(friends[i]);
        QListWidgetItem *item=new QListWidgetItem();
        itemmap.push_back(item);
        item->setSizeHint(QSize(40,40));
        list->addItem((item));
        btnmap.push_back(btn);
        te->setFixedSize(220,40);
        //this->friendlistsss.push_back(te);
        this->friendlistsss.push_back(te);
        connect(btn,&QPushButton::clicked,[=](){
           qDebug()<<btn->text();
           if(isopen[btn->text()]==0)
           {
               qDebug()<<"开聊天";
               isopen[btn->text()]=1;
               QIcon icon1(":/login/prefix1/image/qq.jpg");
               btn->setIcon(icon1);
               chat *ch=new chat(nullptr,si,btn->text(),client,this->username);
               ch->show();
               connect(si,&sig::chufa,ch,&chat::handle);
               connect(si,&sig::guanbi,this,&mainlist::guan);
               //te->show();
           }
        });
        list->setItemWidget(item,te);
    }
    return true;
}

void mainlist::guan(QString name)
{
    //qDebug()<<"??";
    this->isopen[name]=0;
}

void mainlist::logout()
{
    QJsonObject chat;
    chat.insert("type",Logout);
    chat.insert("username",this->username);
    QJsonDocument ree=QJsonDocument(chat);
    QByteArray str=ree.toJson();
    int lenth=strlen(str);
    datahead header={lenth};
    this->client->write((const char*)&header,sizeof(datahead));
    this->client->write(str);
    qDebug()<<" 按了";
}

mainlist::~mainlist()
{
    qDebug()<<"mainlist的析构";
    friendlistsss.clear();
    this->client->close();
    delete ui;
}
