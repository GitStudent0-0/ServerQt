#include "CMDHandler.h"
#include "Command.h"
#include "CommandMessage.h"
#include "CommandStart.h"

CMDHandler::CMDHandler(DataFormat protocol)
  : m_protocol(protocol)
  , m_logger(Logger::getInstance("CMDHandler"))
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
  Protocol* protocol = Protocol::create(m_protocol);
  QByteArray dataMessage = protocol->write(&cmdMessage);
  dataMessage.append('\n');
  QMetaObject::invokeMethod(
      qApp, [protocol]() { delete protocol; }, Qt::QueuedConnection);
  emit sendCmd(dataMessage);
}

void CMDHandler::sortCmd(Command* cmd)
{
  switch (cmd->getCMDType())
  {
  case CMDType::STOP:
  {
    cmd->toDebug(qDebug());
    killTimer(m_timerId);
    break;
  }
  case CMDType::START:
  {
    if (CommandStart* cmdStart = dynamic_cast<CommandStart*>(cmd))
    {
      m_sizePacket = cmdStart->getCount();
      cmdStart->toDebug(qDebug());
      startTimer(5000);
    }
    break;
  }
  default:
    LOG4CPLUS_ERROR(m_logger, "Error: Unknown command");
  }
}