#include "ClientWorker.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

ClientWorker::ClientWorker(qintptr socketDescriptor, Protocol* protocol)
  : socketDescriptor(socketDescriptor)
  , m_logger(Logger::getInstance("Server.ClientWorker"))
  , m_protocol(protocol)
{
  LOG4CPLUS_DEBUG(m_logger, "Memory allocated: " << this << " Current thread ID:"
                                                 << QThread::currentThreadId());
}

ClientWorker::~ClientWorker()
{
  if (m_socket)
  {
    LOG4CPLUS_DEBUG(m_logger,
                    "Memory freed m_socket: " << m_socket << " Current thread ID:"
                                              << QThread::currentThreadId());
    m_socket->deleteLater();
    m_socket = nullptr;
  }

  if (m_cmdHandler)
  {
    LOG4CPLUS_DEBUG(m_logger, "Memory freed m_cmdHandler: "
                                  << m_cmdHandler << " Current thread ID:"
                                  << QThread::currentThreadId());
    m_cmdHandler->deleteLater();
    m_cmdHandler = nullptr;
  }
  m_protocol = nullptr;
  LOG4CPLUS_DEBUG(m_logger, "Memory freed: " << this << " Current thread ID:"
                                             << QThread::currentThreadId());
}

void ClientWorker::process()
{
  int keepIdle = 5;
  int keepInterval = 5;
  int keepCount = 3;
  int userTimeout = 10000;
  m_socket = new QTcpSocket();
  m_cmdHandler = new CMDHandler(m_protocol);
  if (m_socket && m_socket->setSocketDescriptor(socketDescriptor) && m_cmdHandler)
  {
    LOG4CPLUS_DEBUG(m_logger, "Memory allocated m_socket: "
                                  << m_socket << " Current thread ID:"
                                  << QThread::currentThreadId());
    LOG4CPLUS_DEBUG(m_logger, "Memory allocated m_cmdHandler: "
                                  << m_cmdHandler << " Current thread ID:"
                                  << QThread::currentThreadId());

    connect(m_socket, &QTcpSocket::disconnected, this, &ClientWorker::disconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this,
            &ClientWorker::errorOccurred);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientWorker::readyRead);
    connect(this, &ClientWorker::reciveCommand, m_cmdHandler, &CMDHandler::sortCmd);
    connect(m_cmdHandler, &CMDHandler::sendCmd, this, &ClientWorker::sendMessage);

    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    setsockopt(m_socket->socketDescriptor(), IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle,
               sizeof(keepIdle));
    setsockopt(m_socket->socketDescriptor(), IPPROTO_TCP, TCP_KEEPINTVL,
               &keepInterval, sizeof(keepInterval));
    setsockopt(m_socket->socketDescriptor(), IPPROTO_TCP, TCP_KEEPCNT, &keepCount,
               sizeof(keepCount));
    setsockopt(m_socket->socketDescriptor(), IPPROTO_TCP, TCP_USER_TIMEOUT,
               &userTimeout, sizeof(userTimeout));
  }
  else
  {
    LOG4CPLUS_ERROR(m_logger, "Error: Ivalid descriptor: " << socketDescriptor);
    emit finished();
  }
  return;
}

void ClientWorker::disconnected()
{
  LOG4CPLUS_DEBUG(m_logger, "Client disconnected descriptor: "
                                << socketDescriptor
                                << " thread ID: " << QThread::currentThreadId());
  emit finished();
}

void ClientWorker::readyRead()
{
  m_buffer.append(m_socket->readAll());
  char delimiter = '\n';
  while (m_buffer.indexOf(delimiter) != -1)
  {
    int index = m_buffer.indexOf(delimiter);
    QByteArray data = m_buffer.left(index);
    m_buffer.remove(0, index + 1);
    Command* cmd = m_protocol->read(data);
    if (!cmd)
      LOG4CPLUS_ERROR(m_logger, "Error: Сommand not in the specified format");
    else
      emit reciveCommand(cmd);
  }
}

void ClientWorker::sendMessage(const QByteArray& data, size_t size)
{
  if (m_socket->write(data) > 0)
    emit sendTotalBytes(size);
}

void ClientWorker::errorOccurred(QAbstractSocket::SocketError socketError)
{
  if (socketError == QAbstractSocket::RemoteHostClosedError)
  {
    LOG4CPLUS_INFO(m_logger, "Client disconnected correctly: " << socketError);
    return;
  }
  if (socketError == QAbstractSocket::NetworkError ||
      socketError == QAbstractSocket::RemoteHostClosedError ||
      socketError == QAbstractSocket::SocketTimeoutError ||
      socketError == QAbstractSocket::UnknownSocketError)
  {
    LOG4CPLUS_ERROR(m_logger, "Socket error: " << socketError);
    m_socket->abort();
  }
}
