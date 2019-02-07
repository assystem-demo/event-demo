#include "client.hpp"

#include <QDate>

namespace EventDemo {

Client::Client(const QString& server_url, qint16 port, int event_id, QObject* parent)
    : QObject(parent), _event_id(event_id)
{
  _handshaker = std::make_unique<Handshaker>(Handshaker::Type::Client);

  connect(_handshaker.get(), SIGNAL(accepted(QTcpSocket*, uint8_t)), this, SLOT(accepted()));
  connect(_handshaker.get(), SIGNAL(rejected(QTcpSocket*)), this, SLOT(rejected()));

  subscribe(server_url, port);
}

void Client::subscribe(const QString& server_url, qint16 port)
{
  _connection = std::make_unique<QTcpSocket>(this);

  connect(_connection.get(), SIGNAL(connected()), this, SLOT(connected()));
  connect(_connection.get(), SIGNAL(disconnected()), this, SLOT(disconnected()));
  connect(_connection.get(), SIGNAL(readyRead()), this, SLOT(worker()));

  _connection->connectToHost(server_url, port);

  if (!_connection->waitForConnected(5000)) {
    qDebug() << "Error connecting to socket: " << _connection->errorString();
  }
}

void Client::accepted() { qDebug() << "connection established and accepted"; }

void Client::rejected()
{
  qDebug() << "connection was rejected ... disconnecting socket";
  _connection->disconnectFromHost();
}

void Client::worker()
{
  qDebug() << "received event";
  qDebug() << QDate::currentDate().toString();
  qDebug() << _connection->readAll();
}

void Client::connected() { _handshaker->shakeHands(_connection.get(), _event_id); }

void Client::disconnected()
{
  qDebug() << "disconnected...";
  _connection->deleteLater();
}

} // namespace EventDemo
