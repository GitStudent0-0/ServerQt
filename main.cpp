#include <QCoreApplication>
#include <QTimer>
#include <QTcpServer>
#include <QHostAddress>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QTextStream>
#include <QString>

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

    QTcpServer* server = new QTcpServer();


    if (server->listen(QHostAddress::Any, port))
        qDebug() << "Port: "<< port;


    int execResult = a.exec();

    if (server->isListening())
    {
        server->close();
        qDebug() << "server close";
        delete server;
        server = nullptr;
    }

    return execResult;
}
