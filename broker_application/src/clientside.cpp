#include "clientside.hpp"
#include "ConnectionDefinitions.hpp"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtNetwork/QTcpSocket>

namespace EventDemo {

ClientSide::~ClientSide()
{
  std::for_each(std::begin(_clients), std::end(_clients), [](auto& element) {
    std::for_each(std::begin(element.second), std::end(element.second),
                  [](auto* connection) { delete connection; });
  });
}

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
  clientList.push_back(connection);
  qInfo() << "Connection established to client listening for signal"
          << static_cast<unsigned int>(id);
  connect(connection, &QTcpSocket::disconnected, this,
          [this, connection, id]() { onConnectionClosed(connection, id); });
}

void ClientSide::onEvent(uint8_t id)
{
  auto clientList = _clients.find(id);
  if (clientList == _clients.end()) {
    return;
  }
  std::for_each(std::begin(clientList->second), std::end(clientList->second),
                [](auto client) { client->write("\x88"); });
}

void ClientSide::onConnectionClosed(QTcpSocket* connection, uint8_t id)
{
  auto clientList = _clients.find(id);
  if (clientList == _clients.end()) {
    return;
  }

  qInfo() << "Connection closed to client connecting to id" << id;
  auto element =
      std::find(std::begin(clientList->second), std::end(clientList->second), connection);
  if (element != clientList->second.end()) {
    clientList->second.erase(element);
    connection->deleteLater();
  }
}

} // namespace EventDemo
