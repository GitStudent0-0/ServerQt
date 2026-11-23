#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUrl>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;

class HttpServer : public QTcpServer
{
  Q_OBJECT

public:
  HttpServer(QObject* parent, int port);
  ~HttpServer();

private slots:
  void connection();
  void readyRead();
  void error(QAbstractSocket::SocketError socketError);
  void disconnected();

public slots:
  void stats(int count, size_t bytes);

signals:
  void finished();

private:
  Logger m_logger;
  QHash<QTcpSocket*, QByteArray> m_buffers;
  int m_countClients = 0;
  size_t m_bytesSent = 0;
};
#endif // HTTPSERVER_H
