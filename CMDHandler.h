#ifndef CMDHANDLER_H
#define CMDHANDLER_H
#include "Command.h"
#include "Protocol.h"
#include <QObject>
#include <QThread>
#include <climits>
#include <cmath>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;

class CMDHandler : public QObject
{
  Q_OBJECT

public:
  CMDHandler(DataFormat protocol);
  ~CMDHandler();

protected:
  void timerEvent(QTimerEvent* event) override;

public slots:
  void sortCmd(Command* cmd);

signals:
  void sendCmd(const QByteArray& data);

private:
  DataFormat m_protocol;
  Logger m_logger;
  int m_sizePacket;
  int m_timerId;
  double m_currentX = static_cast<double>(INT_MIN);
};

#endif // CMDHANDLER_H
