#include "ClientWorker.h"
#include "CommandMessage.h"
#include "CommandStart.h"
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

void ClientWorker::timerEvent(QTimerEvent* event)
{

  double start = static_cast<double>(INT_MIN);
  double end = static_cast<double>(INT_MAX);
  double step = (end - start) / (m_sizePacket - 1);
  QVector<double> data(m_sizePacket);
  for (int i = 0; i < m_sizePacket; i++)
  {
    double x = start + i * step;
    data[i] = std::sin(x);
  }
  CommandMessage cmdMessage(CMDType::MESSAGE, m_sizePacket, data);
  JsonEncoder encoder;
  QByteArray dataMessage = encoder.writeJson(cmdMessage);
  m_socket->write(dataMessage);
}

void ClientWorker::sendMessage() { startTimer(5000); }

void ClientWorker::stopMessage() { killTimer(m_timerId); }

void ClientWorker::disconnected()
{
  LOG4CPLUS_DEBUG(m_logger, "Client disconnected descriptor: "
                                << socketDescriptor
                                << " thread ID: " << QThread::currentThreadId());
  emit finished();
}

void ClientWorker::readyRead()
{
  // QByteArray data = m_socket->readAll();
  // JsonDecoder decoder;
  // QJsonObject jsonDoc;
  // bool succes = decoder.parseToRawObject(data, jsonDoc);
  // CMDType type = static_cast<CMDType>(jsonDoc["CMDType"].toInt());
  // switch (type)
  // {
  // case CMDType::START:
  // {
  //   CommandStart cmdStart;
  //   if (decoder.readJson(jsonDoc, cmdStart))
  //   {
  //     m_sizePacket = cmdStart.getCount();
  //     cmdStart.toDebug(qDebug());
  //     emit commandMessage();
  //   }
  //   break;
  // }
  // case CMDType::STOP:
  // {
  //   CommandStop cmdStop;
  //   if (decoder.readJson(jsonDoc, cmdStop))
  //   {
  //     // Тут вызываем геттеры, уникальные ТОЛЬКО для команды STOP
  //     cmdStop.toDebug(qDebug());
  //   }
  //   break;
  // }
  // }

  // if (succes && cmdStart.getCMDType() == CMDType::START)
  // {
  //   m_sizePacket = cmdStart.getCount();
  //   cmdStart.toDebug(qDebug());
  //   emit commandMessage();
  // }
}