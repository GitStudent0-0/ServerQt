#ifndef SERVER_H
#define SERVER_H
#include "ClientWorker.h"
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QHash>
#include <QHostAddress>
#include <QObject>
#include <QRandomGenerator>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QVector>
class Server : public QTcpServer
{
    Q_OBJECT

  protected:
    void incomingConnection(qintptr socketDescriptor) override;

  public:
    explicit Server(QObject* parent);
    ~Server();
    void startServer(int port);
  signals:
    void sendMessageToWorker(ClientWorker* target, const QByteArray& block);
  private slots:
    void broadcastMessage();
    void error(const QString& errorMsg);
    void disconnected();

  private:
    QVector<QString> messages;
    QVector<ClientWorker*> m_workers;
};
#endif // SERVER_H
