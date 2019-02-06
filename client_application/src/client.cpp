#include "client.hpp"

#include <QDate>

namespace EventDemo {

Client::Client(const QString& server_url, qint16 port, int event_id, QObject* parent)
    : QObject(parent), event_id(event_id)
{
  handshaker = std::make_unique<Handshaker>(Handshaker::Type::Client);

  connect(handshaker.get(), SIGNAL(accepted(QTcpSocket*)), this, SLOT(accepted()));
  connect(handshaker.get(), SIGNAL(rejected(QTcpSocket*)), this, SLOT(rejected()));

  subscribe(server_url, port);
}

void Client::subscribe(const QString& server_url, qint16 port)
{
  connection = std::make_unique<QTcpSocket>(this);

  connect(connection.get(), SIGNAL(connected()), this, SLOT(connected()));
  connect(connection.get(), SIGNAL(disconnected()), this, SLOT(disconnected()));
  connect(connection.get(), SIGNAL(readyRead()), this, SLOT(worker()));

  connection->connectToHost(server_url, port);

  if (!connection->waitForConnected(5000)) {
    qDebug() << "Error connecting to socket: " << connection->errorString();
  }
}

void Client::accepted() { qDebug() << "connection established and accepted"; }

void Client::rejected()
{
  qDebug() << "connection was rejected ... disconnecting socket";
  connection->disconnectFromHost();
}

void Client::worker()
{
  qDebug() << "received event";
  qDebug() << QDate::currentDate().toString();
  qDebug() << connection->readAll();
}

void Client::connected() { handshaker->shakeHands(connection.get()); }

void Client::disconnected()
{
  qDebug() << "disconnected...";
  connection->deleteLater();
}

} // namespace EventDemo
