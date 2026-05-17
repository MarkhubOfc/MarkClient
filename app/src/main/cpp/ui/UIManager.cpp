#include "UIManager.h"
#include "lib/TextureManager.h"
#include "../Client.h"
#include "../modules/ModuleManager.h"
#include "../utils/Logger.h"
#include <android/asset_manager.h>

void UIManager::init(ANativeWindow* window, int width, int height, void* assetMgr) {
    if (initialized) return;
    screenW = width;
    screenH = height;

    LoadLib::WindowConfig cfg;
    cfg.title = "Mark Client v1.0.0";
    cfg.theme = LoadLib::WindowTheme::Dark;
    cfg.width = 0.9f;
    cfg.height = 0.7f;
    cfg.draggable = true;
    cfg.closable = true;

    mainWindow = std::make_unique<LoadLib::Window>(cfg);
    mainWindow->init(width, height, static_cast<AAssetManager*>(assetMgr));

    setupWindow();

    initialized = true;
    showMenu = true;
    Logger::info("LoadLib UI initialized");
}

void UIManager::setupWindow() {
    auto* mm = Client::get().getModuleManager();
    if (!mm) return;

    const char* catNames[] = {"Combat", "Movement", "Render", "World", "Misc"};
    const char* catIcons[] = {
        "Icon/combat.png",
        "Icon/movement.png",
        "Icon/combat.png", // Fallback para ícones ausentes
        "Icon/movement.png",
        "Icon/combat.png"
    };

    for (int i = 0; i < 5; i++) {
        LoadLib::TabConfig tcfg;
        tcfg.title = catNames[i];
        tcfg.icon = catIcons[i];

        auto tab = std::make_unique<LoadLib::Tab>(tcfg);
        auto* tabPtr = tab.get();

        auto mods = mm->getModulesByCategory((Category)i);
        for (auto* mod : mods) {
            LoadLib::Toggle toggle;
            toggle.label = mod->getName();
            toggle.tooltip = mod->getDescription();
            toggle.value = new bool(mod->isEnabled());
            toggle.onChange = [mod](bool v) {
                if (v != mod->isEnabled()) mod->toggle();
            };
            tabPtr->addToggle(toggle);
        }

        mainWindow->addTab(tabPtr);
        tabs.push_back(std::move(tab));
    }
}

void UIManager::shutdown() {
    if (!initialized) return;
    LoadLib::TextureManager::get().cleanup();
    mainWindow.reset();
    tabs.clear();
    initialized = false;
}

void UIManager::newFrame() {}

void UIManager::render() {
    if (!initialized) return;
    if (showMenu && mainWindow) {
        mainWindow->render();
    }
}

void UIManager::handleTouch(float x, float y, bool down) {
    if (!initialized || !mainWindow) return;
    mainWindow->handleTouch(x, y, down);
}
