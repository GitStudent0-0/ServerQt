#include "ClientWorker.h"

ClientWorker::ClientWorker(quint16 socketDescriptor, Protocol* protocol)
  : socketDescriptor(socketDescriptor)
  , m_logger(Logger::getInstance("Server.ClientWorker"))
  , m_protocol(protocol)
{
  LOG4CPLUS_DEBUG(m_logger, "Memory allocated: " << this << " Current thread ID:"
                                                 << QThread::currentThreadId());
}

ClientWorker::~ClientWorker()
{
  m_socket->deleteLater();
  m_socket = nullptr;
  if (!m_socket)
    LOG4CPLUS_DEBUG(m_logger,
                    "Memory freed m_socket: " << m_socket << " Current thread ID:"
                                              << QThread::currentThreadId());
  m_cmdHandler->deleteLater();
  m_cmdHandler = nullptr;
  if (!m_cmdHandler)
    LOG4CPLUS_DEBUG(m_logger, "Memory freed m_cmdHandler: "
                                  << m_cmdHandler << " Current thread ID:"
                                  << QThread::currentThreadId());
  LOG4CPLUS_DEBUG(m_logger, "Memory freed: " << this << " Current thread ID:"
                                             << QThread::currentThreadId());
}

void ClientWorker::process()
{
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
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientWorker::readyRead);
    connect(this, &ClientWorker::reciveCommand, m_cmdHandler, &CMDHandler::sortCmd);
    connect(m_cmdHandler, &CMDHandler::sendCmd, this, &ClientWorker::sendMessage);
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

void ClientWorker::sendMessage(const QByteArray& data) { m_socket->write(data); }