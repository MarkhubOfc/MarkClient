#include "ModuleManager.h"
#include "combat/Killaura.h"
#include "movement/Fly.h"
#include "../utils/Logger.h"

void ModuleManager::init() {
  modules.reserve(10);
  modules.push_back(std::make_unique<Killaura>());
  modules.push_back(std::make_unique<Fly>());
  Logger::info("Modules initialized");
}

void ModuleManager::tick() {
  for (const auto& m : modules) {
    if (m->isEnabled()) m->onTick();
  }
}

std::vector<Module*> ModuleManager::getModules() {
  std::vector<Module*> r;
  r.reserve(modules.size());
  for (const auto& m : modules) r.push_back(m.get());
  return r;
}

std::vector<Module*> ModuleManager::getModulesByCategory(Category cat) {
  std::vector<Module*> result;
  for (const auto& m : modules) {
    if (m->getCategory() == cat) {
      result.push_back(m.get());
    }
  }
  return result;
}
