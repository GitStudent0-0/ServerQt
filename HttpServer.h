#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;

class HttpServer : public QTcpServer
{

public:
  HttpServer(QObject* parent, int port);
  ~HttpServer();

protected:
  void incomingConnection(qintptr socketDescriptor) override;

private slots:
  void connection();
  void readyRead();
  void error(const QString& errorMsg);
  void disconnected();
  void finished();
  void sendAnswer();

public slots:
  void sendStats(int count, int bytes);

signals:
  void getStats();

private:
  Logger m_logger;
  QHash<QTcpSocket*, QByteArray> m_buffers;
};
#endif // HTTPSERVER_H
