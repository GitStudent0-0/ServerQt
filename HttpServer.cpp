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
    if (socket)
    {
      disconnect(socket, &QTcpSocket::disconnected, this, &HttpServer::disconnected);
      disconnect(socket, &QTcpSocket::errorOccurred, this, &HttpServer::error);
      disconnect(socket, &QTcpSocket::readyRead, this, &HttpServer::readyRead);
      socket->deleteLater();
    }
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
    LOG4CPLUS_INFO(m_logger, "Memory freed socket: " << socket);
    m_buffers.remove(socket);
    socket->deleteLater();
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
        if (url.path() == "/index.html")
        {
          QFile file("/home/marina/Server/Http/index.html");
          if (file.open(QIODevice::ReadOnly))
          {
            QByteArray body = file.readAll();
            QByteArray response;
            response.append("HTTP/1.1 200 OK\r\n");
            response.append("Content-Type: text/html charset=utf-8\r\n");
            response.append("Access-Control-Allow-Origin: *\r\n");
            response.append("Content-Length: " + QByteArray::number(body.size()) +
                            "\r\n");
            response.append("\r\n");
            response.append(body);
            socket->write(response);
          }
        }
        else if (url.path() == "/function.js")
        {
          QFile file("/home/marina/Server/Http/function.js");
          if (file.open(QIODevice::ReadOnly))
          {
            QByteArray body = file.readAll();
            QByteArray response;
            response.append("HTTP/1.1 200 OK\r\n");
            response.append(
                "Content-Type: application/javascript; charset=utf-8\r\n");
            response.append("Access-Control-Allow-Origin: *\r\n");
            response.append("Content-Length: " + QByteArray::number(body.size()) +
                            "\r\n");
            response.append("\r\n");
            response.append(body);
            socket->write(response);
          }
        }
        else if (url.path() == "/getStats")
        {
          QJsonObject jsonObj;
          jsonObj["ClientCount"] = m_countClients;
          jsonObj["BytesSent"] = static_cast<qint64>(m_bytesSent);
          QByteArray body = QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);
          QByteArray response;
          response.append("HTTP/1.1 200 OK\r\n");
          response.append("Content-Type: application/json; charset=utf-8\r\n");
          response.append("Access-Control-Allow-Origin: *\r\n");
          response.append("Content-Length: " + QByteArray::number(body.size()) +
                          "\r\n");
          response.append("\r\n");
          response.append(body);
          socket->write(response);
        }
        else
        {
          QByteArray response;
          response.append("HTTP/1.1 404 Not Found\r\n");
          response.append("Content-Length: 0\r\n");
          response.append("\r\n");
          socket->write(response);
        }
      }
    }
  }
}

void HttpServer::stats(int count, size_t size)
{
  m_countClients = count;
  m_bytesSent = size;
}
