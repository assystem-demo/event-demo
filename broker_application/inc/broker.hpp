#pragma once

#include "clientside.hpp"
#include "generatorside.hpp"

namespace EventDemo {

/*!
 * \brief The broker: connects the client to the generator.
 */
class Broker
{
public:
  /*!
   * Starts both the client-side as well as the generator-side listeners for subsrciption-
   */
  void start();

private:
  GeneratorSide _generator;
  ClientSide _client;
};

} // namespace EventDemo
