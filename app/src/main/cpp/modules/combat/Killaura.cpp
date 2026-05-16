#include "Killaura.h"
#include "../../utils/Logger.h"

Killaura::Killaura() : Module("Killaura", "Auto attack", Category::COMBAT) {}

void Killaura::onEnable() {
  Logger::info("Killaura enabled");
}

void Killaura::onTick() {
}
