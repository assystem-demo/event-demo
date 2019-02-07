#include "broker.hpp"

namespace EventDemo {

void Broker::start()
{
  QObject::connect(&_generator, &GeneratorSide::event, &_client, &ClientSide::onEvent);
  _generator.start();
  _client.start();
}

} // namespace EventDemo
