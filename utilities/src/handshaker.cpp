#include "handshaker.hpp"
#include <QtCore/QTimer>
#include <QtNetwork/QTcpSocket>

namespace {
std::map<EventDemo::Handshaker::Type, QByteArray> names{
    {EventDemo::Handshaker::Type::Generator, "EventDemo:Generator"},
    {EventDemo::Handshaker::Type::Client, "EventDemo:Client"},
    {EventDemo::Handshaker::Type::Broker_in, "EventDemo:BrokerToGenerator"},
    {EventDemo::Handshaker::Type::Broker_out, "EventDemo:BrokerToClient"}};

QByteArray acceptMessage = "\x88";

/*!
 * \brief Represents a state in Handshake.
 * This class represents a state in the handshake process.
 * A state has an enter-state action (sending message or id), and an exit-state
 * action (wait for incoming message from the other side of the connection, then
 * compare it with an expected message or extract the id).
 * An action can be empty.
 */
class HandshakeState : public QObject
{
  Q_OBJECT
public:
  enum class EnterAction
  {
    Empty,
    SendMessage,
    SendId
  };

  enum class ExitAction
  {
    Empty,
    CompareMessage,
    ExpectId
  };

  HandshakeState(EnterAction enter, ExitAction exit, QByteArray send, QByteArray receive);

  void start(QTcpSocket* connection, uint8_t id);

  void doCompare(QTcpSocket* connection, uint8_t id);
  void doId(QTcpSocket* connection, uint8_t id);

  std::shared_ptr<QTimer> getTimer(QTcpSocket* connection);

Q_SIGNALS:
  void accepted(QTcpSocket* connection, uint8_t id);
  void rejected(QTcpSocket* connection);

private:
  EnterAction _enter;
  ExitAction _exit;
  QByteArray _toSend;
  QByteArray _toReceive;
};

HandshakeState::HandshakeState(EnterAction enter, ExitAction exit, QByteArray send,
                               QByteArray receive)
    : QObject(), _enter(enter), _exit(exit), _toSend(send), _toReceive(receive)
{
}

std::shared_ptr<QTimer> HandshakeState::getTimer(QTcpSocket* connection)
{
  auto timer = std::make_shared<QTimer>();
  connect(timer.get(), &QTimer::timeout, this, [this, connection]() {
    disconnect(connection, &QTcpSocket::readyRead, this, nullptr);
    Q_EMIT rejected(connection);
  });
  return timer;
}

void HandshakeState::doCompare(QTcpSocket* connection, uint8_t id)
{
  auto timer = getTimer(connection);
  connect(connection, &QTcpSocket::readyRead, this, [this, connection, id, timer]() {
    disconnect(connection, &QTcpSocket::readyRead, this, nullptr);
    disconnect(timer.get());
    auto bytesRead = connection->readAll();
    if (bytesRead != _toReceive) {
      Q_EMIT rejected(connection);
      return;
    }
    Q_EMIT accepted(connection, id);
  });
}

void HandshakeState::doId(QTcpSocket* connection, uint8_t id)
{
  auto timer = getTimer(connection);
  connect(connection, &QTcpSocket::readyRead, this, [this, connection, id, timer]() {
    disconnect(connection, &QTcpSocket::readyRead, this, nullptr);
    disconnect(timer.get());
    auto bytesRead = connection->readAll();
    if (bytesRead != QByteArray{1, static_cast<char>(id)}) {
      Q_EMIT rejected(connection);
      return;
    }
    Q_EMIT accepted(connection, id);
  });
}

void HandshakeState::start(QTcpSocket* connection, uint8_t id)
{
  // Establish connections first, send message later, else we might miss a response.
  switch (_exit) {
  case ExitAction::CompareMessage: {
    doCompare(connection, id);
  } break;
  case ExitAction::ExpectId: {
    doId(connection, id);
  } break;
  case ExitAction::Empty:
    break;
  };

  switch (_enter) {
  case EnterAction::SendMessage:
    connection->write(_toSend);
    break;
  case EnterAction::SendId:
    connection->write(QByteArray{1, static_cast<char>(id)});
    break;
  case EnterAction::Empty:
    break;
  };

  if (_exit == ExitAction::Empty) {
    Q_EMIT accepted(connection, id);
  }
}

} // namespace

namespace EventDemo {

Handshaker::Handshaker(Type connectionType) : QObject(), _connectionType(connectionType)
{
  switch (_connectionType) {
  case Type::Generator: {
    auto state1 = std::make_shared<HandshakeState>(HandshakeState::EnterAction::Empty,
                                                   HandshakeState::ExitAction::CompareMessage, "",
                                                   ::names[Type::Broker_in]);
    auto state2 = std::make_shared<HandshakeState>(HandshakeState::EnterAction::SendMessage,
                                                   HandshakeState::ExitAction::CompareMessage,
                                                   ::names[Type::Generator], ::acceptMessage);
    auto state3 = std::make_shared<HandshakeState>(HandshakeState::EnterAction::SendId,
                                                   HandshakeState::ExitAction::CompareMessage, "",
                                                   ::acceptMessage);
    connect(state1.get(), &HandshakeState::accepted, state2.get(), &HandshakeState::start);
    connect(state2.get(), &HandshakeState::accepted, state3.get(), &HandshakeState::start);
    connect(state3.get(), &HandshakeState::accepted, this, &Handshaker::accepted);
    connect(state1.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state2.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state3.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    _statemachine.push_back(state1);
    _statemachine.push_back(state2);
    _statemachine.push_back(state3);
    _starter = std::bind(&HandshakeState::start, state1.get(), std::placeholders::_1,
                         std::placeholders::_2);
  } break;
  case Type::Client: {
    auto state1 = std::make_shared<HandshakeState>(HandshakeState::EnterAction::Empty,
                                                   HandshakeState::ExitAction::CompareMessage, "",
                                                   ::names[Type::Broker_out]);
    auto state2 = std::make_shared<HandshakeState>(HandshakeState::EnterAction::SendMessage,
                                                   HandshakeState::ExitAction::CompareMessage,
                                                   ::names[Type::Client], ::acceptMessage);
    auto state3 = std::make_shared<HandshakeState>(HandshakeState::EnterAction::SendId,
                                                   HandshakeState::ExitAction::CompareMessage, "",
                                                   ::acceptMessage);
    connect(state1.get(), &HandshakeState::accepted, state2.get(), &HandshakeState::start);
    connect(state2.get(), &HandshakeState::accepted, state3.get(), &HandshakeState::start);
    connect(state3.get(), &HandshakeState::accepted, this, &Handshaker::accepted);
    connect(state1.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state2.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state3.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    _statemachine.push_back(state1);
    _statemachine.push_back(state2);
    _statemachine.push_back(state3);
    _starter = std::bind(&HandshakeState::start, state1.get(), std::placeholders::_1,
                         std::placeholders::_2);
  } break;
  case Type::Broker_in: {
    auto state1 = std::make_shared<HandshakeState>(
        HandshakeState::EnterAction::SendMessage, HandshakeState::ExitAction::CompareMessage,
        ::names[Type::Broker_in], ::names[Type::Generator]);
    auto state2 =
        std::make_shared<HandshakeState>(HandshakeState::EnterAction::SendMessage,
                                         HandshakeState::ExitAction::ExpectId, ::acceptMessage, "");
    auto state3 =
        std::make_shared<HandshakeState>(HandshakeState::EnterAction::SendMessage,
                                         HandshakeState::ExitAction::Empty, ::acceptMessage, "");
    connect(state1.get(), &HandshakeState::accepted, state2.get(), &HandshakeState::start);
    connect(state2.get(), &HandshakeState::accepted, state3.get(), &HandshakeState::start);
    connect(state3.get(), &HandshakeState::accepted, this, &Handshaker::accepted);
    connect(state1.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state2.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state3.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    _statemachine.push_back(state1);
    _statemachine.push_back(state2);
    _statemachine.push_back(state3);
    _starter = std::bind(&HandshakeState::start, state1.get(), std::placeholders::_1,
                         std::placeholders::_2);
  } break;
  case Type::Broker_out: {
    auto state1 = std::make_shared<HandshakeState>(
        HandshakeState::EnterAction::SendMessage, HandshakeState::ExitAction::CompareMessage,
        ::names[Type::Broker_out], ::names[Type::Client]);
    auto state2 =
        std::make_shared<HandshakeState>(HandshakeState::EnterAction::SendMessage,
                                         HandshakeState::ExitAction::ExpectId, ::acceptMessage, "");
    auto state3 =
        std::make_shared<HandshakeState>(HandshakeState::EnterAction::SendMessage,
                                         HandshakeState::ExitAction::Empty, ::acceptMessage, "");
    connect(state1.get(), &HandshakeState::accepted, state2.get(), &HandshakeState::start);
    connect(state2.get(), &HandshakeState::accepted, state3.get(), &HandshakeState::start);
    connect(state3.get(), &HandshakeState::accepted, this, &Handshaker::accepted);
    connect(state1.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state2.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state3.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    _statemachine.push_back(state1);
    _statemachine.push_back(state2);
    _statemachine.push_back(state3);
    _starter = std::bind(&HandshakeState::start, state1.get(), std::placeholders::_1,
                         std::placeholders::_2);
  } break;
  }
}

void Handshaker::shakeHands(QTcpSocket* connection, uint8_t id) { _starter(connection, id); }

} // namespace EventDemo
#include "handshaker.moc"
