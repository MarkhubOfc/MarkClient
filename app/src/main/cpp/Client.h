#pragma once
#include <memory>
#include <EGL/egl.h>

class ModuleManager;
class ImGuiManager;

class Client {
public:
  static constexpr const char* NAME = "Mark Client";
  static constexpr const char* VERSION = "1.0.0";
  
  static Client& get();
  
  void init();
  void initImGui(EGLDisplay display, EGLSurface surface, int width, int height);
  void shutdown();
  void tick();
  void render();
  void toggleMenu();
  
  ModuleManager* getModuleManager() { return moduleManager.get(); }
  ImGuiManager* getImGuiManager() { return imguiManager.get(); }
  
private:
  Client() = default;
  ~Client() = default;
  
  std::unique_ptr<ModuleManager> moduleManager;
  std::unique_ptr<ImGuiManager> imguiManager;
  bool initialized = false;
  bool imguiInitialized = false;
};
