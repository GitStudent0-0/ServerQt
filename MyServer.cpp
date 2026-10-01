#include "MyServer.h"

MyServer::MyServer(QObject* parent) : QTcpServer(parent)
{
    connect(this, &QTcpServer::newConnection, this, &MyServer::slotNewConnection);
}

MyServer::~MyServer()
{
    if (this->isListening())
    {
        this->close();
        qDebug() << "The server closed the connection";
    }
}

void MyServer::startServer(int port)
{
    if (this->listen(QHostAddress::Any, port))
        qDebug() << "The server is listening on port: "<< port;
}

void MyServer::slotNewConnection()
{
    QTcpSocket* socket = this->nextPendingConnection();
    qDebug() << "Client connected successfully";
    connect(socket, &QTcpSocket::readyRead, this, &MyServer::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &MyServer::slotDisconnected);
}

void MyServer::slotDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket)
        socket->deleteLater();
    qDebug() << "Client disconnected";
}

void MyServer::slotReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket)
    {
        QByteArray data = socket->readAll();
        QString text = QString::fromUtf8(data);
        qDebug() << "Client sent a message: " << text;
    }
}

