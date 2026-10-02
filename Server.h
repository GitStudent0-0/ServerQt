#ifndef SERVER_H
#define SERVER_H
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
  void newConnection();
  void disconnected();
  void readyRead();
};
#endif // SERVER_H
