#include "ClientWorker.h"

ClientWorker::ClientWorker(quint16 socketDescriptor)
  : socketDescriptor(socketDescriptor)
{
  m_socket = nullptr;
  QFile logFile("/home/marina/Server/logs/logFile.txt");
  if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
  {
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
        << "[CONSTRUCTOR] Memory allocated: " << this
        << " Current thread ID: " << QThread::currentThreadId() << Qt::endl;
  }
}

ClientWorker::~ClientWorker()
{
  QFile logFile("/home/marina/Server/logs/logFile.txt");
  if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
  {
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
        << "[DESTRUCTOR] Memory freed: " << this
        << " Current thread ID: " << QThread::currentThreadId() << Qt::endl;
  }
  delete m_socket;
  m_socket = nullptr;
  if (!m_socket)
  {
    QFile logFile("/home/marina/Server/logs/logFile.txt");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
      QTextStream out(&logFile);
      out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
          << "[DELETE m_socket] Memory freed: " << m_socket
          << " Current thread ID: " << QThread::currentThreadId() << Qt::endl;
    }
  }
}

void ClientWorker::process()
{
  m_socket = new QTcpSocket();
  if (m_socket && m_socket->setSocketDescriptor(socketDescriptor))
  {
    QFile logFile("/home/marina/Server/logs/logFile.txt");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
      QTextStream out(&logFile);
      out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
          << "[PROCESS] Memory allocated: " << m_socket
          << " Current thread ID: " << QThread::currentThreadId() << Qt::endl;
    }
    qDebug() << " Current thread ID: " << QThread::currentThreadId();
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientWorker::disconnected);
  }
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