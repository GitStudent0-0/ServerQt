#include "Server.h"

Server::Server(QObject* parent, int port, const DataFormat& protocol)
  : QTcpServer(parent)
  , m_logger(Logger::getInstance("Server.NetWork"))
{
  LOG4CPLUS_INFO(m_logger, "Server created");
  m_protocol = Protocol::create(protocol);

  if (!listen(QHostAddress::Any, port))
  {
    LOG4CPLUS_ERROR(m_logger, "Server: Unable to start the server: "
                                  << errorString().toStdString());
    delete m_protocol;
    m_protocol = nullptr;
    close();
    return;
  }
  LOG4CPLUS_INFO(m_logger, "The server is listening on port: " << port);
}

Server::~Server() { LOG4CPLUS_INFO(m_logger, "Server closed"); }

void Server::incomingConnection(qintptr socketDescriptor)
{
  LOG4CPLUS_INFO(m_logger, "Client connected successfully: " << socketDescriptor);
  QThread* thread = new QThread();
  ClientWorker* worker = new ClientWorker(socketDescriptor, m_protocol);
  worker->moveToThread(thread);
  m_workers.push_back(worker);
  connect(worker, &ClientWorker::sendTotalBytes, this, &Server::sumTotalBytes);
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
  if (worker)
  {
    m_workers.removeAll(worker);
    worker->deleteLater();
  }
}

void Server::sumTotalBytes(size_t size) { m_totalBytesSentClients += size; }

void Server::getStats()
{
  int countClients = m_workers.size();
  emit sendStats(countClients, m_totalBytesSentClients);
}
void Server::stop()
{
  if (isListening())
    close();

  while (!m_workers.isEmpty())
  {
    ClientWorker* worker = m_workers.takeFirst();
    if (worker)
    {
      disconnect(worker, &ClientWorker::finished, this, &Server::disconnected);
      disconnect(worker, &ClientWorker::error, this, &Server::error);
      QThread* thread = worker->thread();
      if (thread && thread != QThread::currentThread())
      {
        worker->deleteLater();
        thread->quit();
        thread->wait();
      }
      else
        worker->deleteLater();
    }
  }
  delete m_protocol;
  m_protocol = nullptr;
  LOG4CPLUS_INFO(m_logger, "Server stopped successfully");
  QCoreApplication::quit();
}