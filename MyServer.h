#ifndef MYSERVER_H
#define MYSERVER_H
#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QObject>
class MyServer : public QTcpServer
{
    Q_OBJECT

public:
    MyServer(QObject* parent);
    ~MyServer();
public:
    void startServer(int port);

private slots:
    void slotNewConnection();
    void slotDisconnected();
    void slotReadyRead();
};
#endif // MYSERVER_H
