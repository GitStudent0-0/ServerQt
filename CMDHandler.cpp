#include "CMDHandler.h"
#include "Command.h"
#include "CommandMessage.h"
#include "CommandStart.h"

CMDHandler::CMDHandler(Protocol* protocol)
  : m_logger(Logger::getInstance("Server.CMDHandler"))
  , m_protocol(protocol)
{
  LOG4CPLUS_DEBUG(m_logger, "Memory allocated: " << this << " Current thread ID:"
                                                 << QThread::currentThreadId());
}

CMDHandler::~CMDHandler()
{
  m_protocol = nullptr;
  LOG4CPLUS_DEBUG(m_logger, "Memory freed: " << this << " Current thread ID:"
                                             << QThread::currentThreadId());
}

void CMDHandler::timerEvent(QTimerEvent* event)
{
  if (m_timerId != 0)
  {
    killTimer(m_timerId);
    m_timerId = 0;
  }
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
  QByteArray dataMessage = m_protocol->write(&cmdMessage);
  dataMessage.append('\n');
  emit sendCmd(dataMessage);
  m_timerId = startTimer(5000);
}

void CMDHandler::sortCmd(Command* cmd)
{
  switch (cmd->getCMDType())
  {
  case CMDType::STOP:
  {
    cmd->toDebug(qDebug());
    if (m_timerId != 0)
    {
      killTimer(m_timerId);
      m_timerId = 0;
    }
    break;
  }
  case CMDType::START:
  {
    if (CommandStart* cmdStart = dynamic_cast<CommandStart*>(cmd))
    {
      m_sizePacket = cmdStart->getCount();
      if (m_sizePacket > 30)
        LOG4CPLUS_ERROR(m_logger, "Error: Count size exceeded");
      else
        m_timerId = startTimer(5000);
    }
    break;
  }
  default:
    LOG4CPLUS_ERROR(m_logger, "Error: Unknown command");
  }
  delete cmd;
  cmd = nullptr;
}