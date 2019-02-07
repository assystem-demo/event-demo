#include "client.hpp"
#include "ConnectionDefinitions.hpp"

#include <QCoreApplication>
#include <QCommandLineParser>

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
                    });

  parser.process(app);

  EventDemo::Client client(EventDemo::address.toString(), EventDemo::clientPort, parser.value("eventId").toInt());

  return app.exec();
}
