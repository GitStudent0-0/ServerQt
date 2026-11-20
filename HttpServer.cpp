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

HttpServer::~HttpServer()
{
  LOG4CPLUS_INFO(m_logger, "HTTP Server closed");
  const QList<QTcpSocket*> sockets = m_buffers.keys();
  for (QTcpSocket* socket : sockets)
  {
    if (socket->isOpen())
      socket->deleteLater();
  }
  m_buffers.clear();
}

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

void HttpServer::error(QAbstractSocket::SocketError socketError)
{
  LOG4CPLUS_ERROR(m_logger, "Error: " << socketError);
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
      buffer.remove(0, index + 4);

      QList<QByteArray> lines = data.split('\n');
      QByteArray requestLine = lines.first().trimmed();
      QList<QByteArray> parts = requestLine.split(' ');
      if (parts.size() >= 2 && parts[0] == "GET")
      {
        QString target(parts[1]);
        QUrl url(target);
        if (url.path() == "/getStats")
        {
          socket->setProperty("waitingStats", true);
          emit getStats();
        }
      }
    }
  }
}

void HttpServer::sendStats(int count, size_t size)
{
  m_countClients = count;
  m_bytesSent = size;
  sendAnswer();
}

void HttpServer::sendAnswer()
{
  QJsonObject jsonObj;
  jsonObj["ClientCount"] = m_countClients;
  jsonObj["BytesSent"] = static_cast<qint64>(m_bytesSent);
  QByteArray body = QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);
  QByteArray response;
  response.append("HTTP/1.1 200 OK\r\n");
  response.append("Content-Type: application/json; charset=utf-8\r\n");
  response.append("Access-Control-Allow-Origin: *\r\n");
  response.append("Content-Length: " + QByteArray::number(body.size()) + "\r\n");
  response.append("\r\n");
  response.append(body);
  const QList<QTcpSocket*> sockets = m_buffers.keys();
  for (QTcpSocket* socket : sockets)
  {
    if (socket->property("waitStats").toBool())
    {
      socket->setProperty("waitingStats", false);
      if (socket->isOpen())
      {
        socket->write(response);
        socket->disconnectFromHost();
        socket->flush();
      }
      m_buffers.remove(socket);
    }
  }
}
