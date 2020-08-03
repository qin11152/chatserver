#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <conio.h>
#include <winsock2.h>
#include <iostream>
#include "mysql.h"
#include <Windows.h>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include <vector>
#include "tcpclient.h"
#include <thread>
#include <algorithm>
#include <memory>
#include <mutex>
using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"libmysql.lib")


//mutex _mutex;



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
	GetRecord
}infoname;

struct datahead
{
	int len;
	datahead(int l) :len(l) {};
};
typedef enum regist
{
	RegisterSuccess,
	RegisterFailed,
	UserExist
}registback;



MYSQL mysql;

bool connect();		//�������ݿ�
bool querydatabase(string table);		//�ȼ���select *
void freeconnect();		//�ر����ݿ�����
bool updatestatu(string user, int statu);		//�����û��Ƿ�����
bool checkstatu(int user);		//����û��Ƿ�����
int getid(string user);		//��ȡ�û�����Ӧid
bool querysingle(string table, string condition, string name);		//�ض���ѯ
bool querysinglefield(string table, string field, string condition, string name);		//�ض���ѯĳһ��
int checkstatus(string user);		//��ѯ�û��Ƿ�����
string querypasswd(string user);		//��ѯ�û����룬��¼ʹ��
int getmaximumid(string table);		//��ȡ�������id���Ա��������
int checkuser(string user);		//����û����Ƿ����
bool updatefriends(string requested, string request_user);		//���º��ѱ�
bool addfriend_cache(string request, string requested);		//���º������󻺴�
vector<string> getfriendscache(string user);		//��ȡ�������󻺴�
bool deletefriendcache(string user);		//ɾ�����ѻ���
vector<string> getlist(string user);		//��ȡ�����б�
int getrecordcount(string user,string chatuser);
int getrecordfinal(string user,string chatuser);
bool updatefinalrecord(string user, string chatuser, string message,int id);
int registe(string user, string passwd);		//ע����
string getrecord(string user, string chatuser);			//��ȡ�û������¼
bool updaterecord(string user, string chatuser, string message);		//���������¼���ݿ�
char* unicodeto8(const wchar_t* unicode);		//תutf8��
wchar_t* utf8tounicode(const char* inform);
string stringtoutf8(string &inform);
string Utf8ToGbk(const char* src_str);
wchar_t* chartowchar(const char* inform);
bool test(string message="Ĭ��");

bool sendmessage(int direction, char* information, const vector<shared_ptr<tcpclient>>& my_client);		//������������Ϣ
bool cleanup(vector<shared_ptr<tcpclient>>& my_client, bool& state);		//�����û�����
bool handlefunc(SOCKET& client, vector<shared_ptr<tcpclient>>& my_client);		//���û����Ӽ�������
void listenfunc(vector<shared_ptr<tcpclient>>& my_client, bool& state);		//�����û���������Ϣ
//void listenfunc1(vector<shared_ptr<tcpclient>>& my_client, bool& state);
//vector<tcpclient> my_client;


int main() {
	char _buffer[bufsize] = {};
	bool state = true;
	vector<shared_ptr<tcpclient>> my_client;
	my_client.reserve(1024);
	connect();         //���������ݿ�����
	WSADATA wsadata;
	//����
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		cout << "wsastartup error\n";
	}

	//����������socket
	unsigned long ul = 1;
	SOCKET server;

	server = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN serveraddr;
	//�󶨵�ַ
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(809);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//�󶨵�ַ
	bind(server, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	//����
	listen(server, 100);

	cout << "��ʼ����\n";
	//��socket��Ϊ������
	int setnio = ioctlsocket(server, FIONBIO, (unsigned long*)&ul);


	//SOCKET* tess = &server;
	//tcpclient* tee = new tcpclient(tess, 100);
	//dp.push_back(tee);


	//�����ͻ���socket
	SOCKET client;




	//�����ͻ��˵�ַ
	SOCKADDR_IN clientaddr;
	int len = sizeof(clientaddr);


	//�������̼߳���m_client��������Ϣ����

	//thread listenthread = thread(listenfunc,ref(my_client),ref(state));
	//listenthread.detach();

		//ѭ������
	while (1)
	{
		if (_kbhit())
		{
			cout << "������\n";
			char ch = cin.get();
			if (ch == 'q')
			{
				break;
			}
		}
		//cout << "��ʼѭ��\n";
		//����Ҫ���ӽ�����socket
		client = accept(server, (SOCKADDR*)&clientaddr, &len);
		//��Ϊ������
		int setnio = ioctlsocket(client, FIONBIO, (unsigned long*)&ul);
		if (client == INVALID_SOCKET)
		{
			//cout << "accept fail\n";
		}
		else
		{
			cout << "connect success\n";
			handlefunc(client, my_client);
		}
		//listenfunc1(my_client, state);
		for (int i = 0; i < my_client.size(); i++)
			/*
			@para
			_buffer ��һ����������socket��������ȡ
			_infobuf ÿ�����ӵĵڶ����������ܴ�
			_lastposition ָ�򻺳���β��λ��
			*/
		{
			if ((my_client[i]->getclient()) == INVALID_SOCKET)
			{
				continue;
			}
			char test[1000] = {};
			int lenth = recv((my_client[i]->getclient()), _buffer, bufsize, 0);
			//int lenth=recv((my_client[i]->getclient()), buffer, sizeof(datahead), 0);
			if (lenth > 0)
			{
				memcpy(my_client[i]->returnbuf() + my_client[i]->returnposition(), _buffer, lenth);
				my_client[i]->setposition(my_client[i]->returnposition() + lenth);
				while (my_client[i]->returnposition() >= sizeof(datahead))
				{
					bool state = true;
					datahead* header = (datahead*)my_client[i]->returnbuf();
					cout << "header��ַ" << header << endl;
					int packlen = 0;
					packlen=header->len;
					cout << "packlen" <<packlen << endl;
					int last = my_client[i]->returnposition();
					if (my_client[i]->returnposition() >= packlen + sizeof(datahead))
					{
						char infom[1024] = {};
						//cout << my_client[i]->returnbuf()+4 << endl;
						strncpy(infom, my_client[i]->returnbuf() + 4, packlen);
						memcpy(my_client[i]->returnbuf(), my_client[i]->returnbuf() + packlen+sizeof(datahead), 10*bufsize-packlen-sizeof(datahead));
						cout << "test:" << infom << endl;
						my_client[i]->setposition(my_client[i]->returnposition() - packlen-sizeof(datahead));
						cout << "�޸��˺�buf:" << my_client[i]->returnbuf() << endl;
						cout << "�޸��˺�last_:" << my_client[i]->returnposition() << endl;
						char* information = (char*)infom;
						rapidjson::Document infor;
						infor.Parse(information);
						int type = infor["type"].GetInt();
						switch (type)
						{
						case Login:  //��¼
						{
							cout << "LOGIN:\n";
							string user = infor["username"].GetString();
							//�ж��û��Ƿ����
							if (checkuser(user) == 1)
							{
								//����û��Ƿ�����
								if (checkstatus(user) < 0)
								{
									char* sendbuffer = (char*)"alreadylogin";
									updatestatu(user, 1);
									send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
									vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
									my_client.erase(iter + i);
									i--;
								}
								else
								{
									string passwd = querypasswd(user);
									if (passwd == infor["passwd"].GetString())
									{
										char* sendbuffer = (char*)"yes";
										updatestatu(user, 1);
										send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
										vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
										my_client.erase(iter + i);
										i--;
									}
									else
									{
										char* sendbuffer = (char*)"error";
										send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
										vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
										my_client.erase(iter + i);
										i--;
									}
								}
							}
							else
							{
								char* sendbuffer = (char*)"user doesn't exist";
								send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
								vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
								my_client.erase(iter + i);
								i--;
							}
							state = false;
						}
						break;
						case Init:  //��ʼ��
						{
							cout << "init:\n";
							string user = infor["username"].GetString();
							int id = getid(user);
							//vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
							my_client[i]->setid(id);
						}
						break;
						case Getlist:
						{
							cout << "getlist\n";
							string user = infor["username"].GetString();
							vector<string> map;
							map = getlist(user);
							rapidjson::StringBuffer jsonbuffer;
							rapidjson::Writer<rapidjson::StringBuffer> writer(jsonbuffer);

							writer.StartObject();

							writer.Key("type");
							writer.Int(Getlist);

							writer.Key("count");
							writer.Int(map.size());

							for (int i = 0; i < map.size(); i++)
							{
								string haoba = "name" + to_string(i);
								writer.Key(haoba.c_str());
								string s = map[i];
								writer.String(s.c_str());
							}
							writer.EndObject();

							string str = jsonbuffer.GetString();

							int m_id = getid(user);

							sendmessage(m_id, (char*)str.c_str(), my_client);
						}
						break;
						case Logout:
						{
							cout << "logout\n";
							string user = infor["username"].GetString();
							int id = getid(user);
							updatestatu(user, 0);
							vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
							cout << "before erase: " << my_client.size() << endl;
							my_client.erase(iter + i);
							cout << "after erase " << my_client.size() << endl;;
							i--;
							state = false;
						}
						break;
						case Chat:
						{
							//cout << information << endl;
							string inform = infor["message"].GetString();
							string user = infor["fromwhere"].GetString();
							string chatuser = infor["towhere"].GetString();
							//updaterecord(user, chatuser, inform);
							if (checkstatus(chatuser) < 0)
							{
								int id = getid(chatuser);
								updaterecord(user, chatuser, inform);
								cout << "��֪" << chatuser << "����Ϣ\n";
								sendmessage(id, information, my_client);
							}
							else
							{
								int num = getrecordcount(user, chatuser);
								if (num == 10)
								{
									//�������ϵ��Ǹ������¼
									int id = getrecordfinal(user, chatuser);
									updatefinalrecord(user, chatuser, inform, id);
								}
								else
								{
									//ֱ�Ӳ��뼴��
									updaterecord(user, chatuser, inform);
								}
							}
							//sendmessage(id, information, my_client);
						}
						break;
						case Addfriend:
						{
							string request_user = infor["request_user"].GetString();
							string requested = infor["requested"].GetString();
							if (checkstatus(requested) > 0)		//�û�����,�������ݿ���
							{
								addfriend_cache(request_user, requested);
							}
							else                           //���ߣ�ֱ�ӷ��͸��û�
							{
								int direction = getid(requested);
								while (!sendmessage(direction, information, my_client))
								{
									sendmessage(direction, information, my_client);
								}
							}
						}
						break;
						case AgreeAdd:
						{
							cout << "AGREEADD\n";
							string requested = infor["requested"].GetString();
							string request_user = infor["request_user"].GetString();
							updatefriends(requested, request_user);
							updatefriends(request_user, requested);
						}
						break;
						case InitRequest:
						{
							cout << "INITREQUEST\n";
							string user = infor["username"].GetString();
							vector<string> map = getfriendscache(user);
							if (map.size() < 1)
							{
								cout << "xiaoyu1\n";
								break;
							}
							rapidjson::StringBuffer jsonbuffer;
							rapidjson::Writer<rapidjson::StringBuffer> writer(jsonbuffer);

							writer.StartObject();

							writer.Key("type");
							writer.Int(InitRequest);

							writer.Key("count");
							writer.Int(map.size());

							for (int i = 0; i < map.size(); i++)
							{
								string haoba = "request" + to_string(i);
								writer.Key(haoba.c_str());
								string s = map[i];
								writer.String(s.c_str());
							}
							writer.EndObject();

							string str = jsonbuffer.GetString();

							int m_id = getid(user);

							sendmessage(m_id, (char*)str.c_str(), my_client);
						}
						break;
						case Register:   //ע��
						{
							string user = infor["username"].GetString();
							string passwd = infor["passwd"].GetString();
							int messa = registe(user, passwd);
							if (messa == UserExist)
							{
								char* sendbuffer = (char*)"user already exist";
								send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
							}
							if (messa == RegisterFailed)
							{
								char* sendbuffer = (char*)"error";
								send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
							}
							if (messa == RegisterSuccess)
							{
								char* sendbuffer = (char*)"yes";
								send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
							}
							vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
							my_client.erase(iter + i);
							i--;
						}
						break;
						case GetRecord:
						{
							string user = infor["username"].GetString();
							string chatuser = infor["chatuser"].GetString();
							string message = getrecord(user, chatuser);
							int m_id = getid(user);

							sendmessage(m_id, (char*)message.c_str(), my_client);
						}
						break;
						default:
							cout << "listen ����\n";
						break;
						}
					}
					if (!state) break;
				}
				/*char* information = (char*)buffer;
				rapidjson::Document infor;
				infor.Parse(information);
				int type = infor["type"].GetInt();
				switch (type)
				{
				
				
				//�޸Ĺ�
				case Login:  //��¼
				{
					cout << "LOGIN:\n";
					string user = infor["username"].GetString();
					//�ж��û��Ƿ����
					if (checkuser(user) == 1)
					{
						//����û��Ƿ�����
						if (checkstatus(user) < 0)
						{
							char* sendbuffer = (char*)"alreadylogin";
							updatestatu(user, 1);
							send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
							vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
							my_client.erase(iter + i);
							i--;
						}
						else
						{
							string passwd = querypasswd(user);
							if (passwd == infor["passwd"].GetString())
							{
								char* sendbuffer = (char*)"yes";
								updatestatu(user, 1);
								send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
								vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
								my_client.erase(iter + i);
								i--;
							}
							else
							{
								char* sendbuffer = (char*)"error";
								send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
								vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
								my_client.erase(iter + i);
								i--;
							}
						}
					}
					else
					{
						char* sendbuffer = (char*)"user doesn't exist";
						send(my_client[i]->getclient(), sendbuffer, strlen(sendbuffer) + 1, 0);
						vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
						my_client.erase(iter + i);
						i--;
					}
				}
				break;
				
				break;
				case Init:  //��ʼ��
				{
					cout << "init:\n";
					string user = infor["username"].GetString();
					int id = getid(user);
					//vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
					my_client[i]->setid(id);
				}
				break;
				default:
					cout << "listen������Ϣ\n";
				}
				break;*/
			}
		}
	}
	closesocket(server);
	WSACleanup();

	//freeconnect();
	cleanup(my_client, state);
	return 0;
}

char* unicodeto8(const wchar_t* unicode)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	char* szutf8 = (char*)malloc(len + 1);
	memset(szutf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szutf8, len, NULL, NULL);
	return szutf8;
}
wchar_t* utf8tounicode(const char* inform)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, inform, strlen(inform), NULL, 0);
	wchar_t* res = new wchar_t[len + 1];
	MultiByteToWideChar(CP_UTF8, 0, inform, strlen(inform), res, len);
	res[len] = '\0';
	return res;
}
string stringtoutf8(string &inform)
{
	int nwlen = MultiByteToWideChar(CP_ACP, 0, inform.c_str(), -1, NULL, 0);
	wchar_t* temp = new wchar_t[nwlen + 1];
	ZeroMemory(temp, nwlen * 2 + 2);
	int len = WideCharToMultiByte(CP_UTF8, 0,temp, -1, NULL, NULL, NULL, NULL);
	char* pbuf = new char[len + 1];
	ZeroMemory(pbuf, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, temp, nwlen, pbuf, len, NULL, NULL);
	string res(pbuf);
	delete[]temp;
	delete[]pbuf;
	temp = NULL;
	pbuf = NULL;
	return res;
}
wchar_t* chartowchar(const char* inform)
{
	wchar_t* res;
	int len = MultiByteToWideChar(CP_ACP, 0, inform, strlen(inform), NULL, 0);
	res = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, inform, strlen(inform), res, len);
	res[len] = '\0';
	return res;
}
bool test(string message)
{
	wchar_t* tt = chartowchar(message.c_str());
	char* utf = unicodeto8(tt);
	string s = utf;
	string quer = "insert into user_password value('"+message+"','1234')";
	mysql_query(&mysql, "set names utf8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "����user_passwordʧ��\n";
		return false;
	}
	return true;
}
string Utf8ToGbk(const char* src_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}
bool connect()
{
	mysql_init(&mysql);
	const char host[] = "localhost";
	const char user[] = "root";
	const char psw[] = "qinbiao9704";
	const char database[] = "test";
	//const char table[] = "scores";
	const int port = 3306;
	if (!(mysql_real_connect(&mysql, host, user, psw, database, port, NULL, 0)))
	{
		cout << "error connect to database:" << mysql_error(&mysql) << endl;
		return false;
	}
	else
	{
		cout << "connect successfully\n";
		return true;
	}
}

void freeconnect()
{
	mysql_close(&mysql);
	//cout << "free\n";
	return;
}


bool querydatabase(string table)
{
	string query = "select * from ";
	query += table;
	//�����ʽ
	mysql_query(&mysql, "set names gbk");

	if (mysql_query(&mysql, query.c_str()))
	{
		cout << "��ѯʧ��\n";
	}
	else
	{
		cout << "��ѯ�ɹ�\n";
	}
	MYSQL_RES* res = mysql_store_result(&mysql);
	if (!res)
	{
		cout << "�޷���ѯ����" << mysql_error(&mysql) << endl;;
		return false;
	}
	cout << mysql_affected_rows(&mysql) << "�����ݷ���\n";

	//�洢�ֶ���Ϣ
	char* fields[32];

	//��ȡ������Ϣ
	int col = mysql_field_count(&mysql);

	for (int i = 0; i < col; i++)
	{
		fields[i] = mysql_fetch_field(res)->name;
	}
	for (int i = 0; i < col; i++)
	{
		cout << fields[i] << "\t";
	}
	cout << endl;

	//�������������ݽṹ
	MYSQL_ROW row;
	while (row = mysql_fetch_row(res))
	{
		for (int i = 0; i < col; i++)
		{
			cout << row[i] << "\t";
		}
		cout << endl;
	}
	mysql_free_result(res);
	return true;

}



bool querysingle(string table, string condition, string name)
{
	string quer = "select * from " + table + " where " + condition + "='" + name + "'";
	mysql_query(&mysql, "set names utf8");

	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "query failed " << mysql_error(&mysql);
		return false;
	}
	else
	{
		cout << "query success\n";
	}


	MYSQL_RES* res;

	/*if (!(res = mysql_store_result(&mysql)))
	{
		cout << "�޷���" << mysql_error(&mysql) << "��ȡ���" << endl;
		return false;
	}*/
	res = mysql_store_result(&mysql);
	//�洢�ֶ���Ϣ
	char* fields[32];

	//��ȡ������Ϣ
	int col = mysql_field_count(&mysql);

	for (int i = 0; i < col; i++)
	{
		fields[i] = mysql_fetch_field(res)->name;
	}
	for (int i = 0; i < col; i++)
	{
		cout << fields[i] << "\t";
	}
	cout << endl;

	//�������������ݽṹ
	MYSQL_ROW row;
	while (row = mysql_fetch_row(res))
	{
		for (int i = 0; i < col; i++)
		{
			cout << row[i] << "\t";
		}
		cout << endl;
	}
	mysql_free_result(res);
	return true;
}



bool querysinglefield(string table, string field, string condition, string name)
{
	string quer = "select " + field + " from " + table + " where " + condition + "='" + name + "'";
	mysql_query(&mysql, "set names utf8");

	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "query failed " << mysql_error(&mysql);
		return false;
	}
	else
	{
		cout << "query success\n";
	}


	MYSQL_RES* res;

	/*if (!(res = mysql_store_result(&mysql)))
	{
		cout << "�޷���" << mysql_error(&mysql) << "��ȡ���" << endl;
		return false;
	}*/
	res = mysql_store_result(&mysql);
	//�洢�ֶ���Ϣ
	char* fields[32];

	//��ȡ������Ϣ
	int col = mysql_field_count(&mysql);

	for (int i = 0; i < col; i++)
	{
		fields[i] = mysql_fetch_field(res)->name;
	}
	for (int i = 0; i < col; i++)
	{
		cout << fields[i] << "\t";
	}
	cout << endl;

	//�������������ݽṹ
	MYSQL_ROW row;
	while (row = mysql_fetch_row(res))
	{
		for (int i = 0; i < col; i++)
		{
			cout << row[i] << "\t";
		}
		cout << endl;
	}
	mysql_free_result(res);
	return true;
}


string querypasswd(string user)
{
	string res;
	string quer = "select passwd from userinfo where username='" + user + "'";
	mysql_query(&mysql, "set names gbk");


	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "��ѯʧ��" << mysql_error(&mysql) << endl;
		return res;
	}
	else
	{
		cout << "��ѯ�û�����ɹ�\n";
	}

	MYSQL_RES* ret;


	/*if (!(ret = mysql_store_result(&mysql)))
	{
		cout << "�޷���" << mysql_error(&mysql) << "��ȡ��Ϣ\n";
		return res;
	}*/

	MYSQL_ROW row;
	ret = mysql_store_result(&mysql);
	row = mysql_fetch_row(ret);
	res = row[0];
	mysql_free_result(ret);
	return res;

}


int checkstatus(string user)
{
	string quer = "select STATU from userinfo where username ='" + user + "'";
	mysql_query(&mysql, "set names utf8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "��ѯ״̬ʧ��\n";
		return -1;
	}

	MYSQL_RES* res;
	MYSQL_ROW row;

	if (!(res = mysql_store_result(&mysql)))
	{
		cout << "�޷���ȡ���\n";
		mysql_free_result(res);
		return -1;
	}
	row = mysql_fetch_row(res);
	int c = atoi(row[0]);

	if (c == 1)
	{
		mysql_free_result(res);
		return -1;
	}
	mysql_free_result(res);
	return 1;
}


int getid(string user)
{
	string quer = "select id from userinfo where username='" + user + "'";
	mysql_query(&mysql, "set names utf8");
	if ((mysql_query(&mysql, quer.c_str())))
	{
		cout << "��ѯʧ��\n";
		return -1;
	}
	else
	{
		cout << "��ȡid�ɹ�\n";
	}
	MYSQL_RES* res;
	MYSQL_ROW row;

	if (!(res = mysql_store_result(&mysql)))
	{
		cout << "�޷���ȡ���\n";
		return -1;
	}
	row = mysql_fetch_row(res);
	int c = atoi(row[0]);
	return c;
}


vector<string> getlist(string user)
{
	vector<string> ret;
	string quer = "select friendname from friends where username ='" + user + "'";
	mysql_query(&mysql, "set names utf8");

	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "��ѯ�����б�ʧ��\n";
	}

	MYSQL_RES* res;
	if (!(res = mysql_store_result(&mysql)))
	{
		cout << "�޷���ȡ�����б��ѯ���\n";
	}

	MYSQL_ROW row;

	while (row = mysql_fetch_row(res))
	{
		string temp = row[0];
		cout << temp << endl;
		ret.push_back(temp);
	}
	mysql_free_result(res);
	return ret;

}

int getrecordfinal(string user,string chatuser)
{
	string quer = "select id from record where (fromwhere = '" + user + "'AND TOWHERE = '" + chatuser + "') OR (fromwhere = '"
		+ chatuser + "'AND TOWHERE = '" + user + "')ORDER BY time limit 0,1";
	mysql_query(&mysql, "set names utf8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "��ȡ�����¼idʧ��\n";
		return 0;
	}

	MYSQL_RES* res;
	MYSQL_ROW row;
	if (!(res = mysql_store_result(&mysql)))
	{
		cout << "��ȡ�����¼id�Ľ��ʧ��\n";
		return 0;
	}
	row = mysql_fetch_row(res);
	int ret = atoi(row[0]);
	mysql_free_result(res);
	return ret;
}

bool updatefinalrecord(string user, string chatuser, string message,int id)
{
	string m_id = to_string(id);
	string quer = "update record SET fromwhere = '" + user + "', towhere = '" + chatuser + "', time = now(), message = '" + message + "' where id ="+m_id;
	cout << quer << endl;
	mysql_query(&mysql, "set names utf8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "�޷������¼����״̬" << mysql_error(&mysql) << endl;
		return false;
	}
	return true;
}

int checkuser(string user)
{
	string quer = "select count(*) from userinfo where username='" + user + "'";
	mysql_query(&mysql, "set names utf8");

	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "��ѯʧ��" << mysql_error(&mysql) << endl;
		return 0;
	}
	else
	{
		cout << "��ѯ�ɹ�\n";
	}

	MYSQL_RES* ret;


	/*if (!(ret = mysql_store_result(&mysql)))
	{
		cout << "�޷���" << mysql_error(&mysql) << "��ȡ��Ϣ\n";
		mysql_free_result(ret);
		return 0;
	}*/
	ret = mysql_store_result(&mysql);
	MYSQL_ROW row;
	row = mysql_fetch_row(ret);
	int num = atoi(row[0]);
	//cout << "row0: " << row[0] << endl;
	if (num == 1)
	{
		mysql_free_result(ret);
		return 1;
	}
	mysql_free_result(ret);
	return 0;
}

bool updatefriends(string requested, string request_user)
{
	int max_id = getmaximumid("friends");
	int current_id = max_id + 1;
	string quer = "insert into friends values(" + to_string(current_id) + ",'" + requested + "','" + request_user + "')";
	mysql_query(&mysql, "set names utf8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "����friends��ʧ��\n";
		return false;
	}
	/*current_id++;
	string quer1 = "insert into friends values(" + to_string(current_id) + ",'" + request_user + "','" + requested + "')";
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "����friends��ʧ��\n";
		return false;
	}*/
	return true;
}

int getmaximumid(string table)
{
	string quer = "select max(id) from " + table;
	mysql_query(&mysql, "set names utf8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "��ѯʧ��" << mysql_error(&mysql) << endl;
		return -1;
	}
	else
	{
		cout << "��ȡ���id�ɹ�\n";
	}


	MYSQL_RES* res;

	if (!(res = mysql_store_result(&mysql)))
	{
		cout << "�޷���ȡ���\n";
		mysql_free_result(res);
		return -10;
	}

	MYSQL_ROW row;
	row = mysql_fetch_row(res);
	int num = atoi(row[0]);
	mysql_free_result(res);
	return num;
}


int registe(string user, string passwd)
{

	regist res;
	if (checkuser(user) == 1)
	{
		cout << "�û��Ѵ���\n";
		res = UserExist;
		return res;
	}
	else
	{
		int id = getmaximumid("userinfo");
		string myid = to_string(id + 1);
		string quer = "insert into userinfo values (" + myid + ", '" + user + "', '" + passwd + "', 0,-999)";
		mysql_query(&mysql, "set names utf8");
		if (mysql_query(&mysql, quer.c_str()))
		{
			cout << "����ʧ��\n";
			res = RegisterFailed;
			return res;
		}
		else
		{
			cout << "ע��ɹ�\n";
			res = RegisterSuccess;
			return res;
		}
	}
}

vector<string> getfriendscache(string user)
{
	vector<string> ret;
	string quer = "select request_user from addfriend_cache where requested='" + user + "'";
	mysql_query(&mysql, "set names utf8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "��ȡaddfriendcacheʧ��\n";
		return ret;
	}

	MYSQL_RES* res;
	MYSQL_ROW row;

	if (!(res = mysql_store_result(&mysql)))
	{
		cout << "��ѯ�˵��޷���ȡaddfriendcache�Ľ��\n";
		return ret;
	}
	while (row = mysql_fetch_row(res))
	{
		string temp = row[0];
		ret.push_back(temp);
	}
	deletefriendcache(user);
	return ret;
}

string getrecord(string user, string chatuser)
{
	rapidjson::StringBuffer jsonbuffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(jsonbuffer);
	string quer = "select fromwhere,time, message from record where (fromwhere = '" + user + "'AND TOWHERE = '" + chatuser + "') OR (fromwhere = '"
		+ chatuser + "'AND TOWHERE = '" + user + "')ORDER BY time desc limit 0,10";
	mysql_query(&mysql, "set names utf8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "��ѯ�����¼ʧ��\n";
		return " ";
	}
	MYSQL_RES* res;
	MYSQL_ROW row;
	if (!(res = mysql_store_result(&mysql)))
	{
		cout << "��ȡ�����¼���ʧ��\n";
		return " ";
	}

	int col = mysql_field_count(&mysql);
	int count = 0;


	writer.StartObject();
	while (row = mysql_fetch_row(res))
	{
		for (int i = 0; i < col; i++)
		{
			if (i == 0)
			{
				string temp = "fromwhere" + to_string(count);
				writer.Key(temp.c_str());
				writer.String(row[0]);
			}
			if (i == 1)
			{
				string temp = "time" + to_string(count);
				writer.Key(temp.c_str());
				writer.String(row[1]);
			}
			if (i == 2)
			{
				string temp = "messa" + to_string(count);
				string tr = row[2];
				string xiwang = Utf8ToGbk(tr.c_str());
				cout << "xiwang:" << xiwang << endl;
				//cout << "�任ǰ" << tr << endl;
				wchar_t* tr1 = chartowchar(tr.c_str());
				//cout << tr1 << endl;
				char* tr2 = unicodeto8(tr1);
				//cout << tr2 << endl;
				writer.Key(temp.c_str());
				writer.String(tr.c_str());
				delete []tr1;
				delete[]tr2;
				tr1 = NULL;
				tr2 = NULL;
			}
		}
		count++;
	}
	writer.Key("count");
	writer.Int(count);
	/*writer.EndObject();

	string str = jsonbuffer.GetString();//�������¼���json��ʽ��������Ϊ�ַ�����Ȼ���װ����һ��json��

	rapidjson::StringBuffer jsonbuffer1;
	rapidjson::Writer<rapidjson::StringBuffer> writer1(jsonbuffer1);*/


	writer.Key("type");
	writer.Int(GetRecord);
	writer.Key("chatuser");
	writer.String(chatuser.c_str());

	writer.EndObject();
	string str1 = jsonbuffer.GetString();
	mysql_free_result(res);
	cout << str1 << endl;
	return str1;
}

int getrecordcount(string user, string chatuser)
{
	string quer = "select fromwhere,time, message from record where (fromwhere = '" + user + "'AND TOWHERE = '" + chatuser + "') OR (fromwhere = '"
		+ chatuser + "'AND TOWHERE = '" + user + "')ORDER BY time desc";
	mysql_query(&mysql, "set names utf-8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "��ѯ�����¼ʧ��\n";
		return 0;
	}
	MYSQL_RES* res;
	MYSQL_ROW row;
	if (!(res = mysql_store_result(&mysql)))
	{
		cout << "��ȡ�����¼���ʧ��\n";
		return 0;
	}

	int col = mysql_field_count(&mysql);
	int count = 0;
	while (row = mysql_fetch_row(res))
	{
		count++;
	}
	return count;
}

bool deletefriendcache(string user)
{
	string quer = "delete from addfriend_cache where requested='" + user + "'";
	mysql_query(&mysql, "set names utf-8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "�޷�ɾ��friendcache\n";
		return false;
	}
	return true;
}

bool updatestatu(string user, int statu)
{
	string quer;
	if (statu == 1)
	{
		quer = "update userinfo set STATU=1 where username='" + user + "'";
	}
	if (statu == 0)
	{
		quer = "update userinfo set STATU=0 where username='" + user + "'";
	}
	mysql_query(&mysql, "set names utf-8");


	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "����״̬ʧ��\n";
		return false;
	}
	else
	{
		cout << "���³ɹ�\n";
		return true;
	}
}

bool updaterecord(string user, string chatuser, string message)
{
	/*wchar_t* tt = chartowchar(message.c_str());
	char* utf = unicodeto8(tt);
	string inform = utf;*/
	cout << "update:" << message << endl;
	int maximumid = getmaximumid("record");
	maximumid++;
	string id = to_string(maximumid);
	string quer = "insert into record values(" + id + ",'" + user + "','" + chatuser + "',now(),'" + message + "')";
	mysql_query(&mysql, "set names utf8");
	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "���������¼��ʧ��\n";
		return false;
	}
	return true;
}

bool addfriend_cache(string request, string requested)
{
	int max_id = getmaximumid("addfriend_cache");
	string currentid = to_string(max_id + 1);
	string quer = "insert into addfriend_cache values( currentid, '" + request + "','" + requested + "')";
	mysql_query(&mysql, "set names utf8");

	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "����addfriend_cacheʧ��\n";
		return false;
	}
	else
	{
		cout << "����addfriend_cache�ɹ�\n";
		return true;
	}

}

bool checkstatu(int user)
{
	string m_id = to_string(user);
	string quer = "select STATU from userinfo where id='" + m_id + "'";
	mysql_query(&mysql, "set names utf8");

	if (mysql_query(&mysql, quer.c_str()))
	{
		cout << "��ѯ״̬ʧ��\n";
		return false;
	}

	MYSQL_RES* res;
	if (!(res = mysql_store_result(&mysql)))
	{
		mysql_free_result(res);
		cout << "�޷���ȡ״̬���\n";
		return false;
	}

	MYSQL_ROW row;
	row = mysql_fetch_row(res);

	int statu = atoi(row[0]);
	mysql_free_result(res);
	return statu == 1;
}


void listenfunc(vector<shared_ptr<tcpclient>>& my_client, bool& state)
{
	while (1)
	{
		//_mutex.lock();
		//cout << ">?\n";
		if (state == false)
		{
			cout << "listen finish\n";
			exit(0);
		}
		//cout << "��ʼlisten\n";
		for (int i = 0; i < my_client.size(); i++)
		{
			char buffer[1024] = { 0 };
			if ((my_client[i]->getclient()) == INVALID_SOCKET)
			{
				continue;
			}
			int lenth = recv((my_client[i]->getclient()), buffer, 1024, 0);
			if (lenth > 0)
			{
				char* information = (char*)buffer;
				rapidjson::Document infor;
				infor.Parse(information);
				int type = infor["type"].GetInt();
				switch (type)
				{
				case Chat:
				{
					int destination = infor["to"].GetInt();
					if (checkstatu(destination))
					{
						while (!sendmessage(destination, information, my_client))
						{
							sendmessage(destination, information, my_client);
						}
					}
					else
					{
						cout << "�û�����\n";  //�洢����
					}
				}
				break;
				case Logout:
				{
					cout << "logout\n";
					string user = infor["username"].GetString();
					int id = getid(user);
					updatestatu(user, 0);
					vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
					cout << "before erase: " << my_client.size() << endl;
					my_client.erase(iter + i);
					cout << "after erase " << my_client.size() << endl;;
					i--;
				}
				break;
				case Getlist:
				{
					cout << "getlist\n";
					/*string user = infor["username"].GetString();
					vector<string> friendlist=getlist(user);  //��ȡ��vector�ṹ�ĺ�����
					rapidjson::StringBuffer jsonbuf;
					rapidjson::Writer<rapidjson::StringBuffer> writer(jsonbuf);

					writer.StartObject();
					writer.Key("type");
					writer.String("listinfor");

					writer.Key("count");
					writer.Int(friendlist.size());

					writer.Key("name");
					writer.StartArray();
					for(int i=friendlist.size()-1;i>=0;i--)
					{
						writer.StartObject();
						writer.Key("name");
						writer.String(friendlist[i].c_str());
						writer.EndObject();
					}
					writer.EndArray();
					writer.EndObject();

					string message = jsonbuf.GetString();
					while (!sendmessage(1, (char*)str.c_str(), my_client))
					{
						sendmessage(1, (char*)str.c_str(), my_client);
					}


					*/
					vector<string> map = { "qinbiao","xyc","lixueying" };
					rapidjson::StringBuffer jsonbuffer;
					rapidjson::Writer<rapidjson::StringBuffer> writer(jsonbuffer);

					writer.StartObject();

					writer.Key("count");
					writer.Int(3);

					writer.Key("names");

					writer.StartArray();


					for (int i = 0; i < 3; i++)
					{
						writer.StartObject();
						writer.Key("name");
						string s = map[i];
						writer.String(s.c_str());
						writer.EndObject();
					}
					writer.EndArray();
					writer.EndObject();

					string str = jsonbuffer.GetString();

					while (!sendmessage(1, (char*)str.c_str(), my_client))
					{
						sendmessage(1, (char*)str.c_str(), my_client);
					}
				}
				break;
				default:
					cout << "listen������Ϣ\n";
				}
				break;
			}
		}
		//_mutex.unlock();
		Sleep(10);
	}
	return;
}

/*void listenfunc1(vector<shared_ptr<tcpclient>>& my_client, bool& state)
{
		cout << ">?\n";
		if (state == false)
		{
			cout << "listen finish\n";
			exit(0);
		}
		//cout << "��ʼlisten\n";
		for (int i = 0; i < my_client.size(); i++)
		{
			char buffer[1024] = { 0 };
			if ((my_client[i]->getclient()) == INVALID_SOCKET)
			{
				continue;
			}
			int lenth = recv((my_client[i]->getclient()), buffer, 1024, 0);
			if (lenth > 0)
			{
				char* information = (char*)buffer;
				rapidjson::Document infor;
				infor.Parse(information);
				int type = infor["type"].GetInt();
				switch (type)
				{
				case Chat:
				{
					int destination = infor["to"].GetInt();
					if (checkstatu(destination))
					{
						while (!sendmessage(destination, information, my_client))
						{
							sendmessage(destination, information, my_client);
						}
					}
					else
					{
						cout << "�û�����\n";  //�洢����
					}
				}
				break;
				case Logout:
				{
					cout << "logout\n";
					string user = infor["username"].GetString();
					int id = getid(user);
					updatestatu(user, 0);
					vector<shared_ptr<tcpclient>>::iterator iter = my_client.begin();
					my_client.erase(iter + i);
					i--;
				}
				break;
				case Getlist:
				{
					string user = infor["username"].GetString();
					int m_id = getid(user);
					*vector<string> friendlist=getlist(user);  //��ȡ��vector�ṹ�ĺ�����
					rapidjson::StringBuffer jsonbuf;
					rapidjson::Writer<rapidjson::StringBuffer> writer(jsonbuf);

					writer.StartObject();
					writer.Key("type");
					writer.String("listinfor");

					writer.Key("count");
					writer.Int(friendlist.size());

					writer.Key("name");
					writer.StartArray();*
					vector<string> map = { "qinbiaos","xyc","lixueying" };
					rapidjson::StringBuffer jsonbuffer;
					rapidjson::Writer<rapidjson::StringBuffer> writer(jsonbuffer);

					writer.StartObject();

					writer.Key("type");
					writer.String("list");

					writer.Key("count");
					writer.Int(3);

					writer.Key("names");

					writer.StartArray();


					for (int i = 0; i < 3; i++)
					{
						writer.StartObject();
						writer.Key("name");
						string s = map[i];
						writer.String(s.c_str());
						writer.EndObject();
					}
					writer.EndArray();
					writer.EndObject();

					string str = jsonbuffer.GetString();

					while (!sendmessage(m_id, (char*)str.c_str(), my_client))
					{
						sendmessage(m_id, (char*)str.c_str(), my_client);
					}
				}
				break;
				default:
					cout << "listen������Ϣ\n";
				}
			}
		}
		return;
}*/


bool sendmessage(int direction, char* information, const vector<shared_ptr<tcpclient>>& my_client) {
	for (int i = 0; i < my_client.size(); i++)
	{
		if (my_client[i]->getid() == direction)
		{
			send((my_client[i]->getclient()), information, strlen(information) + 1, 0);
			cout << "sendmessage\n";
			return true;
		}
	}
	return false;
}


bool handlefunc(SOCKET& client, vector<shared_ptr<tcpclient>>& my_client)
{
	SOCKET m_client = client;
	my_client.push_back(make_shared<tcpclient>(m_client, -99));
	//���ܿͻ��˴����ĵ�¼�����Ϣ
	//cout << "sleep" << endl;
	/*Sleep(100);
	SOCKET m_client = client;
	char buffer[1024] = { 0 };
	//memset(&buffer, 0, strlen(buffer));
	//int lenth=recv(m_client, buffer, 1024, 0);
	int lenth;
	while ((lenth = recv(m_client, buffer, 1024, 0)) <= 0)
	{
		lenth = recv(m_client, buffer, 1024, 0);
	}
	if (lenth <= 0) return false;
	//cout << "buffer:" << buffer << endl;

	cout << "����json��ʽ��Ϣ\n";
	//cout << "handle:" << buffer << endl;
	const char* information = (char*)buffer;
	rapidjson::Document info;
	info.Parse(information);
	string user;
	string passwd;
	//��ȡ������Ϣ

	int c = info["type"].GetInt();
	//��¼
	switch (c)
	{
	case Login:  //��¼
		{
		cout << "LOGIN:\n";
		user = info["username"].GetString();
		//�ж��û��Ƿ����
		if (checkuser(user) == 1)
		{
			//����û��Ƿ�����
			if (checkstatus(user) < 0)
			{
				char* sendbuffer = (char*)"alreadylogin";
				updatestatu(user, 1);
				send(m_client, sendbuffer, strlen(sendbuffer) + 1, 0);
				closesocket(m_client);
			}
			else
			{
				passwd = querypasswd(user);
				if (passwd == info["passwd"].GetString())
				{
					char* sendbuffer = (char*)"yes";
					updatestatu(user, 1);
					send(m_client, sendbuffer, strlen(sendbuffer) + 1, 0);
					closesocket(m_client);
				}
				else
				{
					char* sendbuffer = (char*)"error";
					send(m_client, sendbuffer, strlen(sendbuffer) + 1, 0);
					closesocket(m_client);
				}
			}
		}
		else
		{
			char* sendbuffer = (char*)"user doesn't exist";
			send(m_client, sendbuffer, strlen(sendbuffer) + 1, 0);
			closesocket(m_client);
		}
		}
		break;
	case Register:   //ע��
		{
		user = info["username"].GetString();
		passwd = info["passwd"].GetString();
		int messa = registe(user, passwd);
		if (messa == UserExist)
		{
			char* sendbuffer = (char*)"user already exist";
			send(m_client, sendbuffer, strlen(sendbuffer) + 1, 0);
		}
		if (messa == RegisterFailed)
		{
			char* sendbuffer = (char*)"error";
			send(m_client, sendbuffer, strlen(sendbuffer) + 1, 0);
		}
		if (messa == RegisterSuccess)
		{
			char* sendbuffer = (char*)"yes";
			send(m_client, sendbuffer, strlen(sendbuffer) + 1, 0);
		}
		closesocket(m_client);
		}
		break;
	case Init:  //��ʼ��
		{
		cout << "init:\n";
		string user = info["username"].GetString();
		int id = getid(user);
		my_client.push_back(make_shared<tcpclient>(m_client, id));
		}
		break;
	default:
		cout << "default\n";
	}*/
	return true;
}

bool cleanup(vector<shared_ptr<tcpclient>>& my_client, bool& state)
{
	state = false;
	for (auto i = my_client.begin(); i != my_client.end();)
	{
		i = my_client.erase(i);
	}
	return true;
}
