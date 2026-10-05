#ifndef SERVER_H
#define SERVER_H
#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QHostAddress>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

class Server : public QObject
{
    Q_OBJECT
    QTcpServer* m_server = nullptr;
    QVector<QTcpSocket*> sockets;

  public:
    explicit Server(QObject* parent);
    ~Server();
    void startServer(int port);

  private slots:
    void newConnection();
    void disconnected();
    void readyRead();
};
#endif // SERVER_H
