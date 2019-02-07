#pragma once

#include "handshaker.hpp"

#include <QtCore/QObject>
#include <QtNetwork/QTcpServer>

namespace EventDemo {

/*!
 * \brief Class handling the communication to the clients inside the broker application.
 */
class ClientSide : public QObject
{
  Q_OBJECT

public:
  ClientSide() = default;

  /*!
   * \brief Start listening for clients.
   *
   * This method starts the server and makes it listen for clients.
   */
  void start();

  /*!
   * \brief Emit signals to all connected clients.
   * \param id the id of the signal to send.
   */
  void onEvent(uint8_t id);

private:
  void onConnectionDetected();
  void onConnectionAccepted(QTcpSocket* connection, uint8_t id);

  std::map<uint8_t, std::vector<std::shared_ptr<QTcpSocket>>> _clients;
  std::unique_ptr<QTcpServer> _server;
  Handshaker handshaker{Handshaker::Type::Broker_out};
};
} // namespace EventDemo
