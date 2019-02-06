#pragma once

#include <QObject>
#include <QTcpSocket>
#include <functional>
#include <memory>

#include "handshaker.hpp"

namespace EventDemo {

class Client : public QObject
{
  Q_OBJECT
public:
  explicit Client(const QString& server_url, qint16 port, int event_id, QObject* parent = 0);

public Q_SLOTS:
  void connected();
  void disconnected();
  void accepted();
  void rejected();
  void worker();

private:
  void subscribe(const QString& server_url, qint16 port);

private:
  std::unique_ptr<Handshaker> handshaker;
  std::unique_ptr<QTcpSocket> connection;
  uint8_t event_id;
};

} // namespace EventDemo
