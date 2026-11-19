#include "HttpServer.h"

HttpServer::HttpServer(QObject* parent, int port)
  : QTcpServer(parent)
  , m_logger(Logger::getInstance("Server.Http"))
{
  LOG4CPLUS_INFO(m_logger, "HTTP Server created");
  if (!listen(QHostAddress::Any, port))
  {
    LOG4CPLUS_ERROR(m_logger, "HTTP Server: Unable to start the server: "
                                  << errorString().toStdString());
    close();
    return;
  }
  LOG4CPLUS_INFO(m_logger, "The HTTP Server is listening on port: " << port);
  connect(this, &QTcpServer::newConnection, this, &HttpServer::connection);
}

HttpServer::~HttpServer() { LOG4CPLUS_INFO(m_logger, "HTTP Server closed"); }

void HttpServer::connection()
{
  QTcpSocket* socket = nextPendingConnection();
  if (socket)
  {
    LOG4CPLUS_INFO(m_logger, "Http client connected successfully: "
                                 << socket->socketDescriptor());
    m_buffers.insert(socket, QByteArray());
    connect(this, &HttpServer::finished, this, &HttpServer::disconnected);
    connect(socket, &QTcpSocket::disconnected, this, &HttpServer::disconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &HttpServer::error);
    connect(socket, &QTcpSocket::readyRead, this, &HttpServer::readyRead);
  }
  else
  {
    LOG4CPLUS_ERROR(m_logger,
                    "Error: Ivalid descriptor: " << socket->socketDescriptor());
    emit finished();
  }
  return;
}

void HttpServer::disconnected()
{
  QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
  if (socket)
  {
    LOG4CPLUS_INFO(m_logger, "Http client disconnected descriptor: "
                                 << socket->socketDescriptor()
                                 << "Memory freed socket: " << socket);
    socket->deleteLater();
    socket = nullptr;
  }
}

void HttpServer::error(const QString& errorMsg)
{
  LOG4CPLUS_ERROR(m_logger, "Error: " << errorMsg.toStdString());
}

void HttpServer::readyRead()
{
  QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
  if (socket && m_buffers.contains(socket))
  {
    QByteArray& buffer = m_buffers[socket];
    buffer.append(socket->readAll());
    const char* delimiter = "\r\n\r\n";
    while (buffer.indexOf(delimiter) != -1)
    {
      int index = buffer.indexOf(delimiter);
      QByteArray data = buffer.left(index);
      buffer.remove(0, index + 1);
      emit getStats();
    }
  }
}

void sendAnswer() {}