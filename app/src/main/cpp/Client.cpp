#include "Client.h"
#include "modules/ModuleManager.h"
#include "ui/UIManager.h"
#include "utils/Logger.h"

Client& Client::get() {
    static Client instance;
    return instance;
}

Client::~Client() = default;

void Client::init() {
    if (initialized) return;
    Logger::info("Initializing Mark Client");
    moduleManager = std::make_unique<ModuleManager>();
    moduleManager->init();
    initialized = true;
    Logger::info("Client initialized successfully!");
}

void Client::initImGui(ANativeWindow* window, int width, int height, void* assetMgr) {
    if (imguiInitialized) return;
    uiManager = std::make_unique<UIManager>();
    uiManager->init(window, width, height, assetMgr);
    imguiInitialized = true;
}

void Client::shutdown() {
    if (!initialized) return;
    Logger::info("Shutting down client...");
    if (uiManager) uiManager->shutdown();
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
    uiManager->newFrame();
    uiManager->render();
}

void Client::toggleMenu() {
    if (uiManager) uiManager->toggle();
}
