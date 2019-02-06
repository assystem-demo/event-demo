#include <QCoreApplication>

#include "client.hpp"

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);
  //just a test
  EventDemo::Client c("www.google.de", 80, 8);

  return app.exec();
}
