#include "Client.h"
#include "modules/ModuleManager.h"
#include "ui/ImGuiManager.h"
#include "utils/Logger.h"

Client& Client::get() {
  static Client instance;
  return instance;
}

void Client::init() {
  if (initialized) return;
  
  Logger::info("Initializing Mark Client");
  
  moduleManager = std::make_unique<ModuleManager>();
  moduleManager->init();
  
  initialized = true;
  Logger::info("Client initialized successfully!");
}

void Client::initImGui(EGLDisplay display, EGLSurface surface, int width, int height) {
  if (imguiInitialized) return;
  
  imguiManager = std::make_unique<ImGuiManager>();
  imguiManager->init(display, surface, width, height);
  imguiInitialized = true;
}

void Client::shutdown() {
  if (!initialized) return;
  
  Logger::info("Shutting down client...");
  if (imguiManager) imguiManager->shutdown();
  moduleManager.reset();
  initialized = false;
  imguiInitialized = false;
}

void Client::tick() {
  if (!initialized) return;
  moduleManager->tick();
}

void Client::render() {
  if (!initialized || !imguiInitialized) return;
  imguiManager->newFrame();
  imguiManager->render();
}

void Client::toggleMenu() {
  if (imguiManager) imguiManager->toggle();
}
