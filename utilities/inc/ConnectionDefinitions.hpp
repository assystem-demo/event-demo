#include <QtNetwork/QHostAddress>

namespace EventDemo {

const QHostAddress address = QHostAddress::LocalHost;

constexpr const uint16_t generatorPort = 8081;
constexpr const uint16_t clientPort = 8082;
} // namespace EventDemo
