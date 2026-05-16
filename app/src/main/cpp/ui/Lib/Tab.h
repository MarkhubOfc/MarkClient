#pragma once
#include "Window.h"
#include "TextureManager.h"
#include <functional>
#include <string>
#include <vector>

namespace LoadLib {

class Window;

struct Button {
    std::string label;
    std::string tooltip;
    std::function<void()> onClick;
    bool enabled = true;
};

struct Toggle {
    std::string label;
    std::string tooltip;
    bool* value;
    std::function<void(bool)> onChange;
};

struct Slider {
    std::string label;
    float* value;
    float min, max;
    std::function<void(float)> onChange;
};

struct TabConfig {
    std::string title = "Tab";
    std::string icon = "";
};

class Tab {
public:
    Tab(const TabConfig& cfg);
    virtual ~Tab() = default;

    const std::string& getName() const { return config.title; }
    const std::string& getIcon() const { return config.icon; }

    void addButton(const Button& btn);
    void addToggle(const Toggle& toggle);
    void addSlider(const Slider& slider);

    virtual void render(const Rect& contentBounds, Window* window);
    virtual void handleTouch(float x, float y, bool down, const Rect& contentBounds);

    void drawIcon(const Rect& rect, Window* window);

protected:
    TabConfig config;
    std::vector<Button> buttons;
    std::vector<Toggle> toggles;
    std::vector<Slider> sliders;
    Texture* iconTexture = nullptr;

    void loadIcon();
    void drawButton(const Button& btn, const Rect& rect, Window* window, bool hovered);
    void drawToggle(const Toggle& toggle, const Rect& rect, Window* window, bool hovered);
    void drawSlider(const Slider& slider, const Rect& rect, Window* window, bool hovered);
};

}
