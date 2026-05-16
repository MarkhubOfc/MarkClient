#pragma once
#include "Module.h"
#include <vector>
#include <memory>

class ModuleManager {
public:
  void init();
  void tick();
  std::vector<Module*> getModules();
  std::vector<Module*> getModulesByCategory(Category cat);
  
private:
  std::vector<std::unique_ptr<Module>> modules;
};
