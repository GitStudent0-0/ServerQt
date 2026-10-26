#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H
#include "Command.h"
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
  ClientWorker(quint16 socketDescriptor);
  ~ClientWorker();

protected:
  void timerEvent(QTimerEvent* event) override;

public slots:
  void process();
  void sendMessage();
  void disconnected();
  void readyRead();
  void stopMessage();

signals:
  void finished();
  void error(QString err);
  void commandMessage();
  void commandStop();
  void reciveCommand(Command* cmd);

private:
  QTcpSocket* m_socket;
  quint16 socketDescriptor;
  Logger m_logger;
  QByteArray m_buffer;
};

#endif // CLIENTWORKER_H
