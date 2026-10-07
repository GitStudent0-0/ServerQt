#ifndef SERVER_H
#define SERVER_H
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QHash>
#include <QHostAddress>
#include <QObject>
#include <QRandomGenerator>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QVector>
class Server : public QObject
{
    Q_OBJECT
    QTcpServer* m_server = nullptr;
    QVector<QString> messages;
    QHash<qintptr, QTcpSocket*> m_sockets;

  public:
    explicit Server(QObject* parent);
    ~Server();
    void startServer(int port);

  private slots:
    void sendMessage();
    void disconnected();
    void broadcastMessage();
};
#endif // SERVER_H
