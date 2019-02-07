#include "generatorside.hpp"
#include "ConnectionDefinitions.hpp"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtNetwork/QTcpSocket>

namespace EventDemo {

void GeneratorSide::start()
{
  connect(&handshaker, &Handshaker::accepted, this, &GeneratorSide::onConnectionAccepted);
  _server = std::make_unique<QTcpServer>();
  connect(_server.get(), &QTcpServer::newConnection, this, &GeneratorSide::onConnectionDetected);
  _server->listen(address, generatorPort);
}

void GeneratorSide::onConnectionDetected()
{
  if (!_server->hasPendingConnections()) {
    return;
  }
  auto* connection = _server->nextPendingConnection();

  handshaker.shakeHands(connection, 0xFF);
}

void GeneratorSide::onConnectionAccepted(QTcpSocket* connection, uint8_t id)
{
  auto existing = _generators.find(id);
  if (existing != _generators.end()) {
    connection->write("\xFF");
    connection->deleteLater();
    qWarning() << "Rejecting connection to provider for signal" << static_cast<unsigned int>(id);
    return;
  }
  _generators.emplace(std::make_pair(id, std::shared_ptr<QTcpSocket>(connection)));
  connect(connection, &QTcpSocket::readyRead, this, [id, this]() { onEvent(id); });
  qInfo() << "Connection established to provider for signal" << static_cast<unsigned int>(id);
}

void GeneratorSide::onEvent(uint8_t id)
{
  qInfo() << QTime::currentTime().toString() << "Event" << static_cast<unsigned int>(id);
  Q_EMIT event(id);
}

} // namespace EventDemo
