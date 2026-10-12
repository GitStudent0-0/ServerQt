#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H
#include <QDebug>
#include <QObject>
#include <QTcpSocket>
#include <QTextStream>
#include <QThread>
#include <log4cplus/configurator.h>
#include <log4cplus/initializer.h>
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
private slots:
signals:
  void finished();
  void error(QString err);

private:
  QTcpSocket* m_socket;
  quint16 socketDescriptor;
  Logger m_logger = Logger::getInstance(LOG4CPLUS_TEXT("ClientWorker"));
};

#endif // CLIENTWORKER_H
