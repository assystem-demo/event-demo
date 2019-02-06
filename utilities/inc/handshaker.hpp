#pragma once

#include <QObject>

class QTcpSocket;

namespace EventDemo {

class Handshaker : public QObject
{
  Q_OBJECT

public:
  enum class Type
  {
    Generator,
    Client,
    Broker_in,
    Broker_out,
  };

  Handshaker(Type connectionType);

  void shakeHands(QTcpSocket* connection);

Q_SIGNALS:

  void accepted(QTcpSocket* connection);
  void rejected(QTcpSocket* connection);

private:
  Type _connectionType;
};

} // namespace EventDemo
