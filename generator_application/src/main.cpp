#include "generator.hpp"

#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);
  QStringList args = app.arguments();

  QCommandLineParser parser;
  parser.setApplicationDescription("Event Client");
  parser.addHelpOption();
  parser.addOptions({
      {{"i", "eventId"},
       QCoreApplication::translate("main", "Provide an <eventId>"),
       QCoreApplication::translate("main", "eventId")},
      {{"a", "address"},
       QCoreApplication::translate("main", "Provide an <address>"),
       QCoreApplication::translate("main", "address")},
      {{"t", "timer-interval"},
       QCoreApplication::translate("main", "Provide a <timer-interval> in ms"),
       QCoreApplication::translate("main", "timer-interval")},
  });

  parser.process(app);
  EventDemo::Generator generator(parser.value("eventId").toInt());

  QTimer::singleShot(0, &generator, [&parser, &generator]() {
    generator.start(parser.value("address"), parser.value("timer-interval").toInt());
  });
  return app.exec();
}
