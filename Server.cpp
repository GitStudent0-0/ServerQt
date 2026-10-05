#include "Server.h"

Server::Server(QObject* parent)
  : QObject(parent)
{
  m_server = new QTcpServer(this);
  connect(m_server, &QTcpServer::newConnection, this, &Server::newConnection);
}

Server::~Server()
{
  if (m_server->isListening())
    m_server->close();
}

void Server::startServer(int port)
{
  if (m_server->listen(QHostAddress::Any, port))
    qDebug() << "The server is listening on port: " << port;
}

void Server::newConnection()
{
  QTcpSocket* socket = m_server->nextPendingConnection();
  qintptr socketDecriptor = socket->socketDescriptor();
  connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
  connect(socket, &QTcpSocket::readyRead, this, &Server::readyRead);
  connect(socket, &QTcpSocket::disconnected, this, &Server::disconnected);
  sockets.push_back(socket);
  qDebug() << "Client connected successfully: " << socketDecriptor
           << "Socket size: " << sockets.size();
}

void Server::disconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket)
    {
      sockets.removeOne(socket);
      socket->deleteLater();
    }
    qDebug() << "Client disconnected";
}

void Server::readyRead()
{
  QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
  if (socket)
  {
    QByteArray data = socket->readAll();
    QString text = QString::fromUtf8(data);
    qDebug() << "Client sent a message: " << text;
    socket->write("Server processed: " + data);
    socket->flush();
  }
}

// void Server::readyRead()
// {
//   QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
//   if (!socket)
//     return;
//   QDataStream in(socket);
//   in.setVersion(QDataStream::Qt_5_15);
//   while (true)
//   {
//     in.startTransaction();
//     QString text;
//     in >> text;
//     if (in.commitTransaction())
//       qDebug() << "Client sent a message: " << text;
//     else
//       break;
//   }
// }
