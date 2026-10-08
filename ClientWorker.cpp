#include "ClientWorker.h"

ClientWorker::ClientWorker(quint16 socketDescriptor)
  : socketDescriptor(socketDescriptor)
{}

void ClientWorker::process()
{
  socket = new QTcpSocket(this);
  if (!socket->setSocketDescriptor(socketDescriptor))
    return;
  qDebug() << "Current thread ID: " << QThread::currentThreadId();
  connect(socket, &QTcpSocket::disconnected, this, &ClientWorker::disconnected);
  return;
}

void ClientWorker::sendMessage(const QByteArray& block)
{
  if (socket && socket->isOpen())
    socket->write(block);
}

void ClientWorker::disconnected()
{
  qDebug() << "Client disconnected descriptor: " << socketDescriptor
           << "thread ID: " << QThread::currentThreadId();
  emit finished();
}