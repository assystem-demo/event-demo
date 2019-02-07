#include "client.hpp"

#include <QDate>

namespace EventDemo {

Client::Client(const QString& serverUrl, qint16 port, int id, QObject* parent)
    : QObject(parent), _id(id)
{
  _handshaker = std::make_unique<Handshaker>(Handshaker::Type::Client);

  connect(_handshaker.get(), SIGNAL(accepted(QTcpSocket*, uint8_t)), this, SLOT(onAccepted()));
  connect(_handshaker.get(), SIGNAL(rejected(QTcpSocket*)), this, SLOT(onRejected()));

  subscribe(serverUrl, port);
}

void Client::subscribe(const QString& serverUrl, qint16 port)
{
  _connection = std::make_unique<QTcpSocket>(this);

  connect(_connection.get(), SIGNAL(connected()), this, SLOT(onConnected()));
  connect(_connection.get(), SIGNAL(disconnected()), this, SLOT(onDisconnected()));
  connect(_connection.get(), SIGNAL(readyRead()), this, SLOT(onMessageReceived()));

  _connection->connectToHost(serverUrl, port);

  if (!_connection->waitForConnected(5000)) {
    qWarning() << "Error connecting to socket: " << _connection->errorString();
    _connection->deleteLater();
  }
}

void Client::onAccepted() { qInfo() << "connection established and accepted"; }

void Client::onRejected()
{
  qInfo() << "connection was rejected ... disconnecting socket";
  _connection->disconnectFromHost();
}

void Client::onMessageReceived()
{
  qInfo() << "received event";
  qInfo() << QDate::currentDate().toString();
  qInfo() << _connection->readAll();
}

void Client::onConnected() { _handshaker->shakeHands(_connection.get(), _id); }

void Client::onDisconnected()
{
  qInfo() << "disconnected...";
  _connection->deleteLater();
}

} // namespace EventDemo
