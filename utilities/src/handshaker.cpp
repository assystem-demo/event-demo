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

class HandshakeState : public QObject
{
  Q_OBJECT
public:
  HandshakeState(QByteArray send, QByteArray receive);
  void startWithWait(QTcpSocket* connection, uint8_t id);
  void startWithSendForID(QTcpSocket* connection, uint8_t id);
  void startWithSend(QTcpSocket* connection, uint8_t id);
  void startWithSendId(QTcpSocket* connection, uint8_t id);
  void sendonly(QTcpSocket* connection, uint8_t id);

  std::shared_ptr<QTimer> getTimer(QTcpSocket* connection);

Q_SIGNALS:
  void accepted(QTcpSocket* connection, uint8_t id);
  void rejected(QTcpSocket* connection);

private:
  QByteArray _toSend;
  QByteArray _toReceive;
};

HandshakeState::HandshakeState(QByteArray send, QByteArray receive)
    : QObject(), _toSend(send), _toReceive(receive)
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

void HandshakeState::startWithWait(QTcpSocket* connection, uint8_t id)
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

void HandshakeState::startWithSend(QTcpSocket* connection, uint8_t id)
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
  connection->write(_toSend);
}

void HandshakeState::sendonly(QTcpSocket* connection, uint8_t id)
{
  Q_UNUSED(id)
  connection->write(_toSend);
}

void HandshakeState::startWithSendForID(QTcpSocket* connection, uint8_t id)
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
  connection->write(_toSend);
}

void HandshakeState::startWithSendId(QTcpSocket* connection, uint8_t id)
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
  connection->write(QByteArray{1, static_cast<char>(id)});
}

} // namespace

namespace EventDemo {

Handshaker::Handshaker(Type connectionType) : QObject(), _connectionType(connectionType)
{
  switch (_connectionType) {
  case Type::Generator: {
    auto state1 = std::make_shared<HandshakeState>("", ::names[Type::Broker_in]);
    auto state2 = std::make_shared<HandshakeState>(::names[Type::Generator], ::acceptMessage);
    auto state3 = std::make_shared<HandshakeState>("", ::acceptMessage);
    connect(state1.get(), &HandshakeState::accepted, state2.get(), &HandshakeState::startWithWait);
    connect(state2.get(), &HandshakeState::accepted, state3.get(),
            &HandshakeState::startWithSendId);
    connect(state3.get(), &HandshakeState::accepted, this, &Handshaker::accepted);
    connect(state1.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state2.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state3.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    _statemachine.push_back(state1);
    _statemachine.push_back(state2);
    _statemachine.push_back(state3);
    _starter = std::bind(&HandshakeState::startWithWait, state1.get(), std::placeholders::_1,
                         std::placeholders::_2);
  } break;
  case Type::Client: {
    auto state1 = std::make_shared<HandshakeState>("", ::names[Type::Broker_out]);
    auto state2 = std::make_shared<HandshakeState>(::names[Type::Client], ::acceptMessage);
    auto state3 = std::make_shared<HandshakeState>("", ::acceptMessage);
    connect(state1.get(), &HandshakeState::accepted, state2.get(), &HandshakeState::startWithWait);
    connect(state2.get(), &HandshakeState::accepted, state3.get(),
            &HandshakeState::startWithSendId);
    connect(state3.get(), &HandshakeState::accepted, this, &Handshaker::accepted);
    connect(state1.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state2.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state3.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    _statemachine.push_back(state1);
    _statemachine.push_back(state2);
    _statemachine.push_back(state3);
    _starter = std::bind(&HandshakeState::startWithWait, state1.get(), std::placeholders::_1,
                         std::placeholders::_2);
  } break;
  case Type::Broker_in: {
    auto state1 =
        std::make_shared<HandshakeState>(::names[Type::Broker_in], ::names[Type::Generator]);
    auto state2 = std::make_shared<HandshakeState>(::acceptMessage, "");
    auto state3 = std::make_shared<HandshakeState>(::acceptMessage, "");
    connect(state1.get(), &HandshakeState::accepted, state2.get(),
            &HandshakeState::startWithSendForID);
    connect(state2.get(), &HandshakeState::accepted, state3.get(), &HandshakeState::sendonly);
    connect(state3.get(), &HandshakeState::accepted, this, &Handshaker::accepted);
    connect(state1.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state2.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state3.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    _statemachine.push_back(state1);
    _statemachine.push_back(state2);
    _statemachine.push_back(state3);
    _starter = std::bind(&HandshakeState::startWithSend, state1.get(), std::placeholders::_1,
                         std::placeholders::_2);
  } break;
  case Type::Broker_out: {
    auto state1 =
        std::make_shared<HandshakeState>(::names[Type::Broker_out], ::names[Type::Client]);
    auto state2 = std::make_shared<HandshakeState>(::acceptMessage, "");
    auto state3 = std::make_shared<HandshakeState>(::acceptMessage, "");
    connect(state1.get(), &HandshakeState::accepted, state2.get(),
            &HandshakeState::startWithSendForID);
    connect(state2.get(), &HandshakeState::accepted, state3.get(), &HandshakeState::sendonly);
    connect(state3.get(), &HandshakeState::accepted, this, &Handshaker::accepted);
    connect(state1.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state2.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    connect(state3.get(), &::HandshakeState::rejected, this, &Handshaker::rejected);
    _statemachine.push_back(state1);
    _statemachine.push_back(state2);
    _statemachine.push_back(state3);
    _starter = std::bind(&HandshakeState::startWithSend, state1.get(), std::placeholders::_1,
                         std::placeholders::_2);
  } break;
  }
}

void Handshaker::shakeHands(QTcpSocket* connection, uint8_t id) { _starter(connection, id); }

} // namespace EventDemo
#include "handshaker.moc"
