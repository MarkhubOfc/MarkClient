#include "ModuleManager.h"
#include "combat/Killaura.h"
#include "movement/Fly.h"
#include "../utils/Logger.h"

void ModuleManager::init() {
  modules.push_back(std::make_unique<Killaura>());
  modules.push_back(std::make_unique<Fly>());
  Logger::info("Loaded %zu modules", modules.size());
}

void ModuleManager::tick() {
  for (auto& m : modules) {
    if (m->isEnabled()) m->onTick();
  }
}

std::vector<Module*> ModuleManager::getModules() {
  std::vector<Module*> r;
  for (auto& m : modules) r.push_back(m.get());
  return r;
}
