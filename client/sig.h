#ifndef SIG_H
#define SIG_H

#include <QWidget>

class sig : public QWidget
{
    Q_OBJECT
public:
    explicit sig(QWidget *parent = nullptr);

signals:
    void chufa(QString name,QString info);
    void guanbi(QString name);
    void infor(QString name);

};

#endif // SIG_H
