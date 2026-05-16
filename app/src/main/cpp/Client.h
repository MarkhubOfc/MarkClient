#pragma once
#include <memory>
#include <android/native_window.h>

class ModuleManager;
class UIManager;

class Client {
public:
    static constexpr const char* NAME = "Mark Client";
    static constexpr const char* VERSION = "1.0.0";
    static Client& get();
    void init();
    void initImGui(ANativeWindow* window, int width, int height, void* assetMgr);
    void shutdown();
    void tick();
    void render();
    void toggleMenu();
    ModuleManager* getModuleManager() { return moduleManager.get(); }
    UIManager* getUIManager() { return uiManager.get(); }

private:
    Client() = default;
    ~Client() = default;
    std::unique_ptr<ModuleManager> moduleManager;
    std::unique_ptr<UIManager> uiManager;
    bool initialized = false;
    bool imguiInitialized = false;
};
