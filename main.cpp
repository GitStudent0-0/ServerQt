#include "Server.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QHostAddress>
#include <QString>
#include <QTcpServer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption portOption("p","Port","default-port");
    QCommandLineOption protocolOption("protocol","Protocol","default-protocol");
    parser.addOption(portOption);
    parser.addOption(protocolOption);
    parser.process(a);

    int port = 0;
    if (parser.isSet(portOption))
        port = parser.value(portOption).toInt();


    QString protocol;
    if (parser.isSet(protocolOption))
        protocol = parser.value(protocolOption);

    MyServer* server = new MyServer(qApp);
    server->startServer(port);
    return a.exec();
}
