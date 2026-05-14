#include "Fly.h"
#include "../../utils/Logger.h"

Fly::Fly() : Module("Fly", "Allows flight", Category::MOVEMENT) {}

void Fly::onEnable() {
  Logger::info("Fly enabled");
}
