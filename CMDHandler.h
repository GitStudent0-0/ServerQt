#ifndef CMDHANDLER_H
#define CMDHANDLER_H
#include "Command.h"
#include "Protocol.h"
#include <QCoreApplication>
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
  CMDHandler(Protocol* protocol);
  ~CMDHandler();

protected:
  void timerEvent(QTimerEvent* event) override;

public slots:
  void sortCmd(Command* cmd);

signals:
  void sendCmd(const QByteArray& data);

private:
  Logger m_logger;
  Protocol* m_protocol = nullptr;
  int m_sizePacket;
  int m_timerId;
  double m_currentX = static_cast<double>(INT_MIN);
};

#endif // CMDHANDLER_H
