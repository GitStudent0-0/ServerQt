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
#include <atomic>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using std::atomic;

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
  void sumTotalBytes(size_t size);

signals:
  void sendStats(int count, size_t bytes);

private:
  QVector<ClientWorker*> m_workers;
  Logger m_logger;
  Protocol* m_protocol;
  atomic<size_t> m_totalBytesSentClients{0};
};
#endif // SERVER_H
