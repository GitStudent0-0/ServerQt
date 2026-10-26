#include "CMDHandler.h"
#include "Command.h"
#include "CommandMessage.h"
#include "CommandStart.h"
#include "JsonDecoder.h"
#include "JsonEncoder.h"

CMDHandler::CMDHandler()
  : m_logger(Logger::getInstance("CMDHandler"))
{
  LOG4CPLUS_DEBUG(m_logger, "Memory allocated: " << this << " Current thread ID:"
                                                 << QThread::currentThreadId());
}

CMDHandler::~CMDHandler()
{
  LOG4CPLUS_DEBUG(m_logger, "Memory freed: " << this << " Current thread ID:"
                                             << QThread::currentThreadId());
}

void CMDHandler::timerEvent(QTimerEvent* event)
{
  double start = static_cast<double>(INT_MIN);
  double end = static_cast<double>(INT_MAX);
  double step = (end - start) / (m_sizePacket - 1);
  QVector<double> data(m_sizePacket);
  for (int i = 0; i < m_sizePacket; i++)
  {
    data[i] = sin(m_currentX);
    m_currentX += step;
  }
  CommandMessage cmdMessage(CMDType::MESSAGE, m_sizePacket, data);
  JsonEncoder encoder;
  QByteArray dataMessage = encoder.writeJson(cmdMessage);
  dataMessage.append('\n');
  m_socket->write(dataMessage);
}
