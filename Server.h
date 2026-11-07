#ifndef SERVER_H
#define SERVER_H
#include "ClientWorker.h"
#include "Protocol.h"
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
  explicit Server(QObject* parent, int port, const DataFormat& protocol);
  ~Server();

private slots:
  void error(const QString& errorMsg);
  void disconnected();
  void stop();

private:
  QVector<ClientWorker*> m_workers;
  Logger m_logger;
  Protocol* m_protocol;
};
#endif // SERVER_H
