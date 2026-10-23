#include "Server.h"

Server::Server(QObject* parent)
  : QTcpServer(parent)
  , m_logger(Logger::getInstance("Server"))
{
  messages << tr("You've been leading a dog's life. Stay off the furniture.")
           << tr("You've got to think about tomorrow.")
           << tr("You will be surprised by a loud noise.")
           << tr("You will feel hungry again in another hour.")
           << tr("You might have mail.")
           << tr("You cannot kill time without injuring eternity.")
           << tr("Computers are not intelligent. They only think they are.");
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
  m_protocol = protocol;
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
  QString message = messages[QRandomGenerator::global()->bounded(messages.size())];
  LOG4CPLUS_DEBUG(m_logger, "Client connected successfully: " << socketDescriptor);
  QThread* thread = new QThread();
  ClientWorker* worker = new ClientWorker(socketDescriptor);
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