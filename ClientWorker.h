#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H
#include "CommandStart.h"
#include "JsonDecoder.h"
#include "JsonEncoder.h"
#include <QDebug>
#include <QObject>
#include <QTcpSocket>
#include <QTextStream>
#include <QThread>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
using namespace log4cplus;
class ClientWorker : public QObject
{
  Q_OBJECT

public:
  ClientWorker(quint16 socketDescriptor);
  ~ClientWorker();
public slots:
  void process();
  void sendMessage(ClientWorker* target, const QByteArray& block);
  void disconnected();
  void readyRead();

private slots:
signals:
  void finished();
  void error(QString err);
  void commandMessage();

private:
  QTcpSocket* m_socket;
  quint16 socketDescriptor;
  Logger m_logger;
};

#endif // CLIENTWORKER_H
