#ifndef REGIST_H
#define REGIST_H

#include <QWidget>
#include "ui_regist.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include "tcpclient.h"

struct datahead
{
    int len;
    datahead(int l):len(l){};
};


namespace Ui {
class regist;
}

class regist : public QWidget
{
    Q_OBJECT

public:
    explicit regist(QWidget *parent = nullptr);
    ~regist();


private:
    Ui::regist *ui;
};

#endif // REGIST_H
