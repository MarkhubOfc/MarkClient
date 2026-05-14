#include "Client.h"
#include "modules/ModuleManager.h"
#include "utils/Logger.h"

Client& Client::get() {
  static Client instance;
  return instance;
}

void Client::init() {
  Logger::info("Initializing Mark Client");
  moduleManager = std::make_unique<ModuleManager>();
  moduleManager->init();
}

void Client::shutdown() {
  moduleManager.reset();
}
