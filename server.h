#ifndef SERVER_H
#define SERVER_H
#include <QCoreApplication>
#include <QTcpServer>
#include <QHostAddress>
#include <QDebug>
#include <QObject>
class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    MyServer(){}
    ~MyServer(){}

    signals:
    void newCoonection();
};
#endif // SERVER_H
