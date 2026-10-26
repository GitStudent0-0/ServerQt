#include "ClientWorker.h"
#include "JsonDecoder.h"
#include "JsonEncoder.h"

ClientWorker::ClientWorker(quint16 socketDescriptor)
  : socketDescriptor(socketDescriptor)
  , m_logger(Logger::getInstance("ClientWorker"))
{
  m_socket = nullptr;
  LOG4CPLUS_DEBUG(m_logger, "Memory allocated: " << this << " Current thread ID:"
                                                 << QThread::currentThreadId());
}

ClientWorker::~ClientWorker()
{
  LOG4CPLUS_DEBUG(m_logger, "Memory freed: " << this << " Current thread ID:"
                                             << QThread::currentThreadId());
  delete m_socket;
  m_socket = nullptr;
  if (!m_socket)
    LOG4CPLUS_DEBUG(m_logger, "Memory freed: " << m_socket << " Current thread ID:"
                                               << QThread::currentThreadId());
}

void ClientWorker::process()
{
  m_socket = new QTcpSocket();
  if (m_socket && m_socket->setSocketDescriptor(socketDescriptor))
  {
    LOG4CPLUS_DEBUG(m_logger, "Memory allocated: " << m_socket
                                                   << " Current thread ID:"
                                                   << QThread::currentThreadId());
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientWorker::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientWorker::readyRead);
    connect(this, &ClientWorker::commandMessage, this, &ClientWorker::sendMessage);
    connect(this, &ClientWorker::commandStop, this, &ClientWorker::stopMessage);
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

    Command* cmd = JsonDecoder::getCmd(data);

    emit reciveCommand(cmd);
  }
  //   JsonDecoder decoder;
  //   QJsonObject jsonDoc; //ЗАТУПИЛА

  //   if (decoder.parseToRawObject(data, jsonDoc))
  //   {
  //     CMDType type = static_cast<CMDType>(jsonDoc["CMDType"].toInt());
  //     switch (cmd->getCMDType())
  //     {
  //     case CMDType::START:
  //     {
  //       CommandStart *cmdStart = dynamic_cast<CommandStart> (cmd);
  //       cmdStart->getCount();

  //       if (decoder.readJson(jsonDoc, cmdStart))
  //       {
  //         m_sizePacket = cmdStart.getCount();
  //         cmdStart.toDebug(qDebug());
  //         emit commandMessage();
  //       }
  //       break;
  //     }
  //     case CMDType::STOP:
  //     {
  //       Command cmdStop;
  //       if (decoder.readJson(jsonDoc, cmdStop))
  //       {
  //         cmdStop.toDebug(qDebug());
  //         emit commandStop();
  //       }
  //       break;
  //     }
  //     }
  //   }
  // }
}