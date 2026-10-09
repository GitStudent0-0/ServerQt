#include "ClientWorker.h"

ClientWorker::ClientWorker(quint16 socketDescriptor)
  : socketDescriptor(socketDescriptor)
{}

ClientWorker::~ClientWorker() {}

void ClientWorker::process()
{
  m_socket = new QTcpSocket(this);
  if (!m_socket->setSocketDescriptor(socketDescriptor))
    return;
  qDebug() << "Current thread ID: " << QThread::currentThreadId();
  connect(m_socket, &QTcpSocket::disconnected, this, &ClientWorker::disconnected);
  return;
}

void ClientWorker::sendMessage(ClientWorker* target, const QByteArray& block)
{
  if (target != this)
    return;
  if (m_socket && m_socket->isOpen())
    m_socket->write(block);
}

void ClientWorker::disconnected()
{
  qDebug() << "Client disconnected descriptor: " << socketDescriptor
           << "thread ID: " << QThread::currentThreadId();
  emit finished();
}