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
  QCoreApplication a(argc, argv);
  QCommandLineParser parser;
  QCommandLineOption portOption("p", "Port", "default-port");
  QCommandLineOption protocolOption("protocol", "Protocol", "default-protocol");
  parser.addOption(portOption);
  parser.addOption(protocolOption);
  parser.process(a);

  int port = 0;
  if (parser.isSet(portOption))
    port = parser.value(portOption).toInt();

  QString protocol;
  if (parser.isSet(protocolOption))
    protocol = parser.value(protocolOption);

  server = new Server(qApp);
  server->startServer(port, protocol);

  return a.exec();
}
