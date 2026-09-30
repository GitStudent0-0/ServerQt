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
    QCommandLineOption portParse(QStringList()<<"p"<<"port","Port","default-port");
    QCommandLineOption protocolParse(QStringList()<<"protocol"<<"protocol","Protocol","default-protocol");
    parser.addOption(portParse);
    parser.addOption(protocolParse);
    qDebug() << "Введите порт и протокол:";
    QTextStream in(stdin);
    QString line = in.readLine();
    QStringList arg = line.split(' ', Qt::SkipEmptyParts);
    arg.prepend(a.applicationName());
    parser.parse(arg);
    if (parser.isSet(portParse))
    {
        int port = parser.value(portParse).toInt();
        qDebug() << "Введенный порт:" << port;
    }
    if (parser.isSet(protocolParse))
    {
        QString protocol = parser.value(protocolParse).toLower();
        qDebug() << "Введенный протокол:" <<  protocol;
    }

    return a.exec();
}
