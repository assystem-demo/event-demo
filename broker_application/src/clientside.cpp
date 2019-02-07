#include "clientside.hpp"
#include "ConnectionDefinitions.hpp"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtNetwork/QTcpSocket>

namespace EventDemo {

void ClientSide::start()
{
  connect(&handshaker, &Handshaker::accepted, this, &ClientSide::onConnectionAccepted);
  _server = std::make_unique<QTcpServer>();
  connect(_server.get(), &QTcpServer::newConnection, this, &ClientSide::onConnectionDetected);
  _server->listen(address, clientPort);
}

void ClientSide::onConnectionDetected()
{
  if (!_server->hasPendingConnections()) {
    return;
  }
  auto* connection = _server->nextPendingConnection();

  handshaker.shakeHands(connection, 0xFF);
}

void ClientSide::onConnectionAccepted(QTcpSocket* connection, uint8_t id)
{
  auto clientList = _clients[id];
  clientList.push_back(std::shared_ptr<QTcpSocket>(connection));
  qInfo() << "Connection established to client listening for signal"
          << static_cast<unsigned int>(id);
}

void ClientSide::onEvent(uint8_t id)
{
  auto clientList = _clients.find(id);
  if (clientList == _clients.end()) {
  }
  std::for_each(std::begin(clientList->second), std::end(clientList->second),
                [](auto client) { client->write("\x88"); });
}

} // namespace EventDemo
