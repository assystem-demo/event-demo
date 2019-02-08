#pragma once

#include "handshaker.hpp"
#include <QObject>
#include <QTimer>
#include <QtNetwork/QTcpSocket>
#include <functional>
#include <memory>

namespace EventDemo {

/*!
 * \brief Generates events and sends them over TCP connection.
 */
class Generator : public QObject
{
  Q_OBJECT

public:
  /*!
   * \brief Constructor
   * \param event_id the Id of the event to generate.
   */
  Generator(uint8_t event_id);

  /*!
   * Call this method to start the generation of events.
   * \param hostAddress the address of the broker,
   * \param interval the time interval for the events in ms.
   */
  void start(QHostAddress hostAddress, int interval);

private:
  void nowConnected();
  void nowClosed();
  void onAccepted(QTcpSocket*);
  void onRejected(QTcpSocket*);
  void onReceived();
  void sendEvent();

  uint8_t _event_id;
  QTimer _timer;

  std::unique_ptr<QTcpSocket> _tcpConnection;
  Handshaker _handshaker{Handshaker::Type::Generator};
};

} // namespace EventDemo
