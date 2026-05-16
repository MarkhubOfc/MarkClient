#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <GLES3/gl3.h>
#include <android/native_window.h>
#include <android/asset_manager.h>

namespace LoadLib {

enum class WindowTheme { Dark, Light };

struct Color {
    float r, g, b, a;
    Color(float r_ = 1.0f, float g_ = 1.0f, float b_ = 1.0f, float a_ = 1.0f)
        : r(r_), g(g_), b(b_), a(a_) {}
};

struct Rect {
    float x, y, w, h;
    Rect(float x_ = 0, float y_ = 0, float w_ = 0, float h_ = 0)
        : x(x_), y(y_), w(w_), h(h_) {}
    bool contains(float px, float py) const {
        return px >= x && px <= x + w && py >= y && py <= y + h;
    }
};

struct WindowConfig {
    std::string title = "Window";
    WindowTheme theme = WindowTheme::Dark;
    float width = 0.9f;
    float height = 0.7f;
    bool draggable = true;
    bool closable = true;
};

class Tab;

class Window {
public:
    Window(const WindowConfig& config);
    ~Window();

    void init(int screenW, int screenH, AAssetManager* assetMgr);
    void render();
    void handleTouch(float x, float y, bool down);
    void addTab(Tab* tab);
    void setVisible(bool visible);
    bool isVisible() const;
    void toggle();

    const WindowConfig& getConfig() const { return config; }
    Rect getBounds() const { return bounds; }
    int getScreenW() const { return screenW; }
    int getScreenH() const { return screenH; }
    Color getThemeColor(const std::string& name) const;

    void drawRect(const Rect& rect, const Color& color);
    void drawBorder(const Rect& rect, const Color& color, float thickness);

private:
    WindowConfig config;
    Rect bounds;
    Rect headerBounds;
    Rect closeBounds;
    Rect contentBounds;
    int screenW, screenH;
    bool visible = true;
    bool dragging = false;
    float dragOffsetX = 0;
    float dragOffsetY = 0;
    int activeTab = 0;
    std::vector<Tab*> tabs;
    std::unordered_map<std::string, Color> themeColors;
    AAssetManager* assetManager = nullptr;

    void setupTheme();
    void updateLayout();
};

}
