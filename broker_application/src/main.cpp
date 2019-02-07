#include "broker.hpp"

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  EventDemo::Broker broker;

  QTimer::singleShot(0, Qt::CoarseTimer, [&broker]() { broker.start(); });
  return app.exec();
}
