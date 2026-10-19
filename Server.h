#ifndef SERVER_H
#define SERVER_H
#include "ClientWorker.h"
#include "CommandStart.h"
#include "JsonDecoder.h"
#include "JsonEncoder.h"
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QHostAddress>
#include <QObject>
#include <QRandomGenerator>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QVector>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
using namespace log4cplus;
class Server : public QTcpServer
{
    Q_OBJECT

  protected:
    void incomingConnection(qintptr socketDescriptor) override;

  public:
    explicit Server(QObject* parent);
    ~Server();
    void startServer(int port, QString protocol);
    CommandStart createMessage();

  protected:
    void timerEvent(QTimerEvent* event) override;
  signals:
    void sendMessageToWorker(ClientWorker* target, const QByteArray& block);
    void sendCommandToWorker(ClientWorker* target, const CommandStart& command);
  private slots:
    void error(const QString& errorMsg);
    void disconnected();

  private:
    QVector<QString> messages;
    QVector<ClientWorker*> m_workers;
    Logger m_logger;
    QString m_protocol = "";
};
#endif // SERVER_H
