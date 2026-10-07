#include "Server.h"

Server::Server(QObject* parent)
  : QObject(parent)
{
  m_server = new QTcpServer(this);
  messages << tr("You've been leading a dog's life. Stay off the furniture.")
           << tr("You've got to think about tomorrow.")
           << tr("You will be surprised by a loud noise.")
           << tr("You will feel hungry again in another hour.")
           << tr("You might have mail.")
           << tr("You cannot kill time without injuring eternity.")
           << tr("Computers are not intelligent. They only think they are.");
  connect(m_server, &QTcpServer::newConnection, this, &Server::sendMessage);
  QTimer* timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &Server::broadcastMessage);
  timer->start(3000);
}

Server::~Server()
{
  if (m_server->isListening())
    m_server->close();
}

void Server::startServer(int port)
{
  if (!m_server->listen(QHostAddress::Any, port))
  {
    qCritical() << QString("Server: Unable to start the server: %1.")
                       .arg(m_server->errorString());
    m_server->close();
    return;
  }
  qDebug() << "The server is listening on port: " << port;
}

void Server::sendMessage()
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_15);
  out << messages[QRandomGenerator::global()->bounded(messages.size())];
  QTcpSocket* socket = m_server->nextPendingConnection();
  if (!socket)
    return;
  qintptr socketDescriptor = socket->socketDescriptor();
  m_sockets.insert(socketDescriptor, socket);
  connect(socket, &QTcpSocket::disconnected, this, &Server::disconnected);
  qDebug() << "Client connected successfully: " << socketDescriptor;
  socket->write(block);
}

void Server::broadcastMessage()
{
  if (m_sockets.empty())
    return;
  for (QTcpSocket* socket : qAsConst(m_sockets))
  {
    if (socket->state() == QTcpSocket::ConnectedState)
    {
      QByteArray block;
      QDataStream out(&block, QIODevice::WriteOnly);
      out.setVersion(QDataStream::Qt_5_15);
      out << messages[QRandomGenerator::global()->bounded(messages.size())];
      socket->write(block);
    }
  }
}

void Server::disconnected()
{
  QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
  if (!socket)
    return;
  quint16 socketDescriptor = m_sockets.key(socket);
  m_sockets.remove(socketDescriptor);
  socket->deleteLater();
  qDebug() << "Client disconnected: " << socketDescriptor;
}