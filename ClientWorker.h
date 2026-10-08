#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H
#include <QDebug>
#include <QObject>
#include <QTcpSocket>
#include <QThread>
class ClientWorker : public QObject
{
  Q_OBJECT
  QTcpSocket* socket;
  quint16 socketDescriptor;

public:
  ClientWorker(quint16 socketDescriptor);
  ~ClientWorker() {};
public slots:
  void process();
  void sendMessage(const QByteArray& block);
  void disconnected();
private slots:
signals:
  void finished();
  void error(QString err);
};

#endif // CLIENTWORKER_H
