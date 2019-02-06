#include "handshaker.hpp"

namespace EventDemo {

Handshaker::Handshaker(Type connectionType) : QObject(), _connectionType(connectionType) {}

void Handshaker::shakeHands(QTcpSocket* connection) {}

} // namespace EventDemo
