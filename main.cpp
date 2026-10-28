#include "Server.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QHostAddress>
#include <QString>
#include <QTcpServer>
#include <log4cplus/configurator.h>
#include <log4cplus/initializer.h>
#include <signal.h>

using namespace log4cplus;

static Server* server = nullptr;

void signalHandler(int sigInt)
{
  if (sigInt == SIGINT || sigInt == SIGTERM)
    if (server)
      QMetaObject::invokeMethod(server, "stop");
}

int main(int argc, char* argv[])
{
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  Initializer initializer;
  PropertyConfigurator::doConfigure("log4cplus.properties");
  Logger m_logger = Logger::getInstance("main");
  QCoreApplication a(argc, argv);
  QCommandLineParser parser;
  QCommandLineOption portOption("p", "Port", "-p");
  QCommandLineOption protocolOption("protocol", "Protocol", "--protocol");
  parser.addOption(portOption);
  parser.addOption(protocolOption);
  parser.addHelpOption();
  parser.addVersionOption();
  parser.process(a);

  if (!parser.isSet(portOption))
  {
    LOG4CPLUS_ERROR(m_logger, "Error: Option "
                                  << portOption.valueName().toStdString()
                                  << " was not specified");
    parser.showHelp(1);
  }
  int port = parser.value(portOption).toInt();

  if (!parser.isSet(protocolOption))
  {
    LOG4CPLUS_ERROR(m_logger, "Error: Option "
                                  << protocolOption.valueName().toStdString()
                                  << " was not specified");
    parser.showHelp(1);
  }
  QString protocol = parser.value(protocolOption);

  server = new Server(qApp);
  server->startServer(port, protocol);

  return a.exec();
}
