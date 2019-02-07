#include "client.hpp"

#include <QCoreApplication>
#include <QDate>

namespace EventDemo {

Client::Client(const QString& serverUrl, qint16 port, uint8_t id, QObject* parent)
    : QObject(parent), _id(id)
{
  _handshaker = std::make_unique<Handshaker>(Handshaker::Type::Client);

  connect(_handshaker.get(), &Handshaker::accepted, this, &Client::onAccepted);
  connect(_handshaker.get(), &Handshaker::rejected, this, &Client::onRejected);

  subscribe(serverUrl, port);
}

void Client::subscribe(const QString& serverUrl, qint16 port)
{
  _connection = std::make_unique<QTcpSocket>(this);

  connect(_connection.get(), &QTcpSocket::connected, this, &Client::onConnected);
  connect(_connection.get(), &QTcpSocket::disconnected, this, &Client::onDisconnected);
  connect(_connection.get(), &QTcpSocket::readyRead, this, &Client::onMessageReceived);

  _connection->connectToHost(serverUrl, port);

  if (!_connection->waitForConnected(5000)) {
    qWarning() << "Error connecting to socket: " << _connection->errorString();
    _connection->deleteLater();
    qApp->quit();
    exit(1);
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
  QCoreApplication::exit(1);
  exit(1);
}

} // namespace EventDemo
