#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "ui_mainwindow.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QPushButton>
#include "regist.h"
#include "tcpclient.h"
#include <QMessageBox>
#include "mainlist.h"





QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool connectserver();
signals:
    void initall();

private:
    Ui::MainWindow *ui;
    QTcpSocket *client;

};
#endif // MAINWINDOW_H
