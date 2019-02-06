#pragma once

#include <QObject>

#include <functional>
#include <memory>
#include <vector>

class QTcpSocket;

namespace EventDemo {

/*!
 * \brief Handshaker for the connections of generator and client with broker.
 *
 * This class performs the handshake between the broker and the connecting
 * entities: client and generator.
 * Use the handshaker in each type.
 * A signal will be emitted for each shakeHands: either accepted or rejected.
 */
class Handshaker : public QObject
{
  Q_OBJECT

public:
  /*!
   * \brief Defines the environment, the Handshaker is used in:
   */
  enum class Type
  {
    //! in the generator,
    Generator,
    //! in the client,
    Client,
    //! in the broker connecting to the generator,
    Broker_in,
    //! in the broker connecting to the client.
    Broker_out,
  };

  /*!
   * \brief Constructor
   * \param connectionType Environment of the Handshaker.
   */
  Handshaker(Type connectionType);

  /*!
   * \brief Call to initiate a handshake with the specified connection.
   * \param connection the connection to shake hands with.
   * \param id the id of the event to provide/subscribe (0xFF in case of broker).
   */
  void shakeHands(QTcpSocket* connection, uint8_t id);

Q_SIGNALS:

  /*!
   * \brief This signal is emitted in case the connection is accepted by the handshaker.
   * \param connection the accepted connection.
   * \param id the id of the event to provide/subscribe in case of broker, 0xFF else.
   */
  void accepted(QTcpSocket* connection, uint8_t id);

  /*!
   * \brief This signal is emitted in case the connection is rejected by the handshaker.
   * The connection is rejected if the transmitted greetings are not as expected or if a timeout
   * occurred. \param connection the rejected connection.
   */
  void rejected(QTcpSocket* connection);

private:
  Type _connectionType;
  std::vector<std::shared_ptr<QObject>> _statemachine;
  std::function<void(QTcpSocket*, uint8_t)> _starter;
};

} // namespace EventDemo
