#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H
#include <QDebug>
#include <QObject>
#include <QTcpSocket>
#include <QThread>
class ClientWorker : public QObject
{
  Q_OBJECT

public:
  ClientWorker(quint16 socketDescriptor);
  ~ClientWorker();
public slots:
  void process();
  void sendMessage(ClientWorker* target, const QByteArray& block);
  void disconnected();
private slots:
signals:
  void finished();
  void error(QString err);

private:
  QTcpSocket* m_socket;
  quint16 socketDescriptor;
};

#endif // CLIENTWORKER_H
