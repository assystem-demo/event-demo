#include "ConnectionDefinitions.hpp"
#include "client.hpp"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QTimer>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);
  QStringList args = app.arguments();

  QCommandLineParser parser;
  parser.setApplicationDescription("Event Client");
  parser.addHelpOption();
  parser.addOptions({
      {{"i", "eventId"},
       QCoreApplication::translate("main", "Provide a an <eventId>"),
       QCoreApplication::translate("main", "eventId")},
      {{"a", "address"},
       QCoreApplication::translate("main", "Provide a an <address>"),
       QCoreApplication::translate("main", "address")},
  });

  parser.process(app);

  EventDemo::Client client(parser.value("eventId").toInt());

  QTimer::singleShot(0, Qt::CoarseTimer, [&client, &parser]() {
    client.subscribe(parser.value("address"), EventDemo::clientPort);
  });
  return app.exec();
}
