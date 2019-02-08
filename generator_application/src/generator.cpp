#include "generator.hpp"
#include "ConnectionDefinitions.hpp"

#include <QCoreApplication>
#include <QtCore/QTime>

namespace EventDemo {

Generator::Generator(uint8_t event_id) : _event_id(event_id)
{
  // Create a TCP connection
  _tcpConnection = std::unique_ptr<QTcpSocket>(new QTcpSocket);
}

void Generator::start(QString hostAddress, int interval)
{
  // make connections to TCP events
  connect(_tcpConnection.get(), &QTcpSocket::connected, this, &Generator::nowConnected);
  connect(_tcpConnection.get(), &QTcpSocket::disconnected, this, &Generator::nowClosed);
  connect(&_handshaker, &Handshaker::accepted, this, &Generator::onAccepted);
  connect(&_handshaker, &Handshaker::rejected, this, &Generator::onRejected);

  // connect to the broker
  _tcpConnection->connectToHost(hostAddress, generatorPort);

  // Define a timer to cyclically send events
  _timer.setSingleShot(false);
  _timer.setInterval(interval);
  connect(&_timer, &QTimer::timeout, this, &Generator::sendEvent);
}

void Generator::nowClosed()
{
  qDebug() << "The event generator was disconnected";
  qApp->quit();
}

void Generator::nowConnected()
{
  qDebug() << "The event generator is connected.";
  qDebug() << "Try to registrer.....";
  // Register at broker
  _handshaker.shakeHands(_tcpConnection.get(), _event_id);
}

void Generator::onAccepted(QTcpSocket*)
{
  qDebug() << "The broker accepted the event generator";
  qDebug() << "Start sending events.....";
  _timer.start();
  // Any message sent from the broker to the generator will mean that the connection was rejected.
  connect(_tcpConnection.get(), &QTcpSocket::readyRead, this, &Generator::onReceived);
}

void Generator::onRejected(QTcpSocket*)
{
  qDebug() << "The broker rejected the event generator";
  qDebug() << "Stopped.";
  qApp->quit();
}

// Send cyclic events
void Generator::sendEvent()
{
  qInfo() << QTime::currentTime().toString() << "| GENERATOR | Event" << _event_id;
  _tcpConnection->write("\x88");
  _tcpConnection->flush();
}

void Generator::onReceived()
{
  qDebug() << "Secondary reject";
  qApp->quit();
}

} // namespace EventDemo
