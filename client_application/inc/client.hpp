#pragma once

#include "handshaker.hpp"

#include <QObject>
#include <QTcpSocket>

#include <functional>
#include <memory>

namespace EventDemo {

/*!
 * \brief Client for connecting to the broker.
 *
 * This class uses the handshaker class in order to connect to the broker.
 * Once successfully connected the client will wait for events with his event_id.
 * This event will be received via TCP and simply printed on commandline.
 */
class Client : public QObject
{
  Q_OBJECT
public:
  /*!
 * \brief Constructor
 * \param id the id of the event to subscribe on the broker
 * \param parent qt object parent
 */
  explicit Client(uint8_t id, QObject* parent = 0);

public Q_SLOTS:
  /*!
 * \brief This slot will be called once a TCP connection is established.
 * This will invoke the handshaker in order to approve the connection.
 */
  void onConnected();

  /*!
   * \brief This slot will be called once a TCP connection was disconnected.
   * Once successfully disconnected this will destroy the TCP Socket.
   */
  void onDisconnected();

  /*!
   * \brief This slot will be called once the handshaker has accepted the connection to the server.
   */
  void onAccepted();

  /*!
   * \brief This slot will be called once the handshaker has rejected the connection to the server.
   * After beeing called the TCP connection will be disconnected.
   */
  void onRejected();

  /*!
   * \brief This slot will be called when a new message was sent over TCP, which is ready to be
   * read.
   * Once received the message will be printed with a timestamp.
   */
  void onMessageReceived();

  /*!
   * \brief This function will start a connection to the given port and ip.
   * \param serverUrl url or ip to a TCP server.
   * \param port a port for connecting to a TCP server.
   */
  void subscribe(const QString& server_url, qint16 port);

private:
  std::unique_ptr<Handshaker> _handshaker;
  std::unique_ptr<QTcpSocket> _connection;
  uint8_t _id;
};

} // namespace EventDemo
