#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H
#include "CMDHandler.h"
#include "Command.h"
#include "Protocol.h"
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
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
  ClientWorker(quint16 socketDescriptor, Protocol* protocol);
  ~ClientWorker();

public slots:
  void process();
  void sendMessage(const QByteArray& data);
  void disconnected();
  void readyRead();

signals:
  void finished();
  void error(QString err);
  void commandStop();
  void reciveCommand(Command* cmd);

private:
  QTcpSocket* m_socket;
  quint16 socketDescriptor;
  Logger m_logger;
  Protocol* m_protocol = nullptr;
  QByteArray m_buffer;
  CMDHandler* m_cmdHandler;
};

#endif // CLIENTWORKER_H
