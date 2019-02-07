#include <QCoreApplication>

#include "client.hpp"

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);
  QStringList args = app.arguments();

  if (args.count() != 4) {
      qWarning() << "Received an invalid number of arguments ... closing the application";
      qWarning() << "Usage:";
      qWarning() << "application serverUrl serverPort eventId";
      return 1;
  }

  //just a test
  EventDemo::Client client(args[1], args[2].toInt(), args[3].toInt());

  return app.exec();
}
