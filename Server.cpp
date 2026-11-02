#include "Server.h"

Server::Server(QObject* parent)
  : QTcpServer(parent)
  , m_logger(Logger::getInstance("Server"))
{
  LOG4CPLUS_DEBUG(m_logger, "Server created");
}

Server::~Server()
{
  if (this->isListening())
  {
    this->close();
    this->deleteLater();
  }
  LOG4CPLUS_DEBUG(m_logger, "Server closed");
}

void Server::startServer(int port, QString protocol)
{
  if (protocol.toUpper() == "JSON")
    m_protocol = DataFormat::JSON;
  else if (protocol.toUpper() == "XML")
    m_protocol = DataFormat::XML;

  if (!this->listen(QHostAddress::Any, port))
  {
    LOG4CPLUS_ERROR(m_logger, "Server: Unable to start the server: "
                                  << this->errorString().toStdString());
    this->close();
    return;
  }
  LOG4CPLUS_DEBUG(m_logger, "The server is listening on port: " << port);
}

void Server::incomingConnection(qintptr socketDescriptor)
{
  LOG4CPLUS_DEBUG(m_logger, "Client connected successfully: " << socketDescriptor);
  QThread* thread = new QThread();
  ClientWorker* worker = new ClientWorker(socketDescriptor, m_protocol);
  worker->moveToThread(thread);
  m_workers.push_back(worker);
  connect(worker, &ClientWorker::finished, this, &Server::disconnected);
  connect(worker, &ClientWorker::error, this, &Server::error);
  connect(thread, &QThread::started, worker, &ClientWorker::process);
  connect(worker, &ClientWorker::finished, thread, &QThread::quit);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);
  thread->start();
}

void Server::error(const QString& errorMsg)
{
  LOG4CPLUS_ERROR(m_logger, "Error: " << errorMsg.toStdString());
}

void Server::disconnected()
{
  ClientWorker* worker = qobject_cast<ClientWorker*>(sender());
  if (!worker)
    return;
  m_workers.removeAll(worker);
  worker->deleteLater();
}

void Server::stop() { QCoreApplication::quit(); }