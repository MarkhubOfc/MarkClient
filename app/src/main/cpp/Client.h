#pragma once
#include <memory>

class ModuleManager;

class Client {
public:
  static Client& get();
  void init();
  void shutdown();
  ModuleManager* getModuleManager() { return moduleManager.get(); }
private:
  Client() = default;
  std::unique_ptr<ModuleManager> moduleManager;
};
