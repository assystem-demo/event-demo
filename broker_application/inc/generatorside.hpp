#pragma once

#include "handshaker.hpp"

#include <QtCore/QObject>
#include <QtNetwork/QTcpServer>

namespace EventDemo {

/*!
 * \brief Class handling the communication to the generators inside the broker application.
 */
class GeneratorSide : public QObject
{
  Q_OBJECT

public:
  GeneratorSide() = default;

  /*!
   * \brief Start listening for generators.
   *
   * This method starts the server and makes it listen for generators.
   */
  void start();

Q_SIGNALS:
  void event(uint8_t id);

private:
  void onConnectionDetected();
  void onConnectionAccepted(QTcpSocket* connection, uint8_t id);

  void onEvent(uint8_t id);

  std::map<uint8_t, std::shared_ptr<QTcpSocket>> _generators;
  std::unique_ptr<QTcpServer> _server;
  Handshaker handshaker{Handshaker::Type::Broker_in};
};
} // namespace EventDemo
