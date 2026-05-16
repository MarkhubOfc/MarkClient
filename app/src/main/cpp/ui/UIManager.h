#pragma once
#include <android/native_window.h>
#include <GLES3/gl3.h>
#include <memory>
#include <vector>

namespace LoadLib {
    class Window;
    class Tab;
}

class UIManager {
public:
    void init(ANativeWindow* window, int width, int height, void* assetMgr);
    void shutdown();
    void newFrame();
    void render();
    bool isVisible() { return showMenu; }
    void toggle() { showMenu = !showMenu; }
    void show() { showMenu = true; }
    void hide() { showMenu = false; }
    void handleTouch(float x, float y, bool down);

private:
    bool showMenu = true;
    bool initialized = false;
    int screenW = 0;
    int screenH = 0;
    std::unique_ptr<LoadLib::Window> mainWindow;
    std::vector<std::unique_ptr<LoadLib::Tab>> tabs;

    void setupWindow();
};
