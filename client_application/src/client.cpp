#include "client.hpp"
#include "ConnectionDefinitions.hpp"

#include <QCoreApplication>
#include <QTime>
#include <QtCore/QCoreApplication>

namespace EventDemo {

Client::Client(uint8_t id, QObject* parent) : QObject(parent), _id(id)
{
  _handshaker = std::make_unique<Handshaker>(Handshaker::Type::Client);

  connect(_handshaker.get(), &Handshaker::accepted, this, &Client::onAccepted);
  connect(_handshaker.get(), &Handshaker::rejected, this, &Client::onRejected);
}

void Client::subscribe(const QString& serverUrl, qint16 port)
{
  _connection = std::make_unique<QTcpSocket>(this);

  connect(_connection.get(), &QTcpSocket::connected, this, &Client::onConnected);
  connect(_connection.get(), &QTcpSocket::disconnected, this, &Client::onDisconnected);
  _connection->connectToHost(serverUrl, clientPort);

  if (!_connection->waitForConnected(5000)) {
    qWarning() << "Error connecting to socket: " << _connection->errorString();
    qApp->quit();
  }
}

void Client::onAccepted()
{
  qInfo() << "connection established and accepted";
  connect(_connection.get(), &QTcpSocket::readyRead, this, &Client::onMessageReceived);
}

void Client::onRejected()
{
  qInfo() << "connection was rejected ... disconnecting socket";
  _connection->disconnectFromHost();
}

void Client::onMessageReceived()
{
  qInfo() << QTime::currentTime().toString() << "| CLIENT | Event" << static_cast<unsigned>(_id);
}

void Client::onConnected() { _handshaker->shakeHands(_connection.get(), _id); }

void Client::onDisconnected()
{
  qInfo() << "The client was disconnected.";
  qApp->quit();
}

} // namespace EventDemo
