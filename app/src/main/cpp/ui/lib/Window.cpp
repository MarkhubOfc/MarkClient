#include "Window.h"
#include "Tab.h"
#include "TextureManager.h"
#include "../../utils/Logger.h"

namespace LoadLib {

static const char* vertexShader = R"(
    #version 300 es
    layout(location = 0) in vec2 aPos;
    uniform vec2 uResolution;
    uniform vec2 uOffset;
    uniform vec2 uScale;
    void main() {
        vec2 pos = (aPos * uScale + uOffset) / uResolution * 2.0 - 1.0;
        gl_Position = vec4(pos.x, -pos.y, 0.0, 1.0);
    }
)";

static const char* fragmentShader = R"(
    #version 300 es
    precision mediump float;
    uniform vec4 uColor;
    out vec4 fragColor;
    void main() {
        fragColor = uColor;
    }
)";

static GLuint shaderProg = 0;
static GLuint vao = 0;
static GLuint vbo = 0;

static GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    return s;
}

static void initGL() {
    if (shaderProg) return;
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vs);
    glAttachShader(shaderProg, fs);
    glLinkProgram(shaderProg);
    glDeleteShader(vs);
    glDeleteShader(fs);

    float verts[] = {0,0, 1,0, 0,1, 1,1};
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

Window::Window(const WindowConfig& config) : config(config) {}

Window::~Window() {}

void Window::init(int sw, int sh, AAssetManager* assetMgr) {
    screenW = sw;
    screenH = sh;
    assetManager = assetMgr;
    TextureManager::get().setAssetManager(assetMgr);
    initGL();
    setupTheme();
    updateLayout();
}

void Window::setupTheme() {
    if (config.theme == WindowTheme::Dark) {
        themeColors["bg"] = Color(0.00f, 0.00f, 0.00f, 0.98f);
        themeColors["header"] = Color(0.00f, 0.00f, 0.00f, 1.0f);
        themeColors["headerActive"] = Color(0.00f, 0.47f, 0.84f, 1.0f);
        themeColors["content"] = Color(0.05f, 0.05f, 0.05f, 1.0f);
        themeColors["border"] = Color(0.00f, 0.47f, 0.84f, 1.0f);
        themeColors["text"] = Color(1.0f, 1.0f, 1.0f, 1.0f);
        themeColors["textDisabled"] = Color(0.5f, 0.5f, 0.5f, 1.0f);
        themeColors["tabInactive"] = Color(0.08f, 0.08f, 0.08f, 1.0f);
        themeColors["tabActive"] = Color(0.00f, 0.47f, 0.84f, 1.0f);
        themeColors["button"] = Color(0.10f, 0.10f, 0.10f, 1.0f);
        themeColors["buttonHover"] = Color(1.00f, 0.84f, 0.00f, 0.8f);
        themeColors["buttonActive"] = Color(1.00f, 0.84f, 0.00f, 1.0f);
        themeColors["accent"] = Color(1.00f, 0.84f, 0.00f, 1.0f);
    } else {
        themeColors["bg"] = Color(0.95f, 0.95f, 0.95f, 0.95f);
        themeColors["header"] = Color(0.9f, 0.9f, 0.9f, 1.0f);
        themeColors["headerActive"] = Color(0.0f, 0.47f, 0.84f, 1.0f);
        themeColors["content"] = Color(0.92f, 0.92f, 0.92f, 1.0f);
        themeColors["border"] = Color(0.0f, 0.47f, 0.84f, 1.0f);
        themeColors["text"] = Color(0.1f, 0.1f, 0.1f, 1.0f);
        themeColors["textDisabled"] = Color(0.5f, 0.5f, 0.5f, 1.0f);
        themeColors["tabInactive"] = Color(0.85f, 0.85f, 0.85f, 1.0f);
        themeColors["tabActive"] = Color(0.0f, 0.47f, 0.84f, 1.0f);
        themeColors["button"] = Color(0.8f, 0.8f, 0.8f, 1.0f);
        themeColors["buttonHover"] = Color(1.0f, 0.84f, 0.0f, 0.8f);
        themeColors["buttonActive"] = Color(1.0f, 0.84f, 0.0f, 1.0f);
        themeColors["accent"] = Color(1.0f, 0.84f, 0.0f, 1.0f);
    }
}

Color Window::getThemeColor(const std::string& name) const {
    auto it = themeColors.find(name);
    if (it != themeColors.end()) return it->second;
    return Color(1, 1, 1, 1);
}

void Window::updateLayout() {
    float w = screenW * config.width;
    float h = screenH * config.height;
    if (w > 900) w = 900;
    if (h > 600) h = 600;
    if (w < 300) w = 300;
    if (h < 200) h = 200;

    bounds = Rect((screenW - w) * 0.5f, (screenH - h) * 0.5f, w, h);
    headerBounds = Rect(bounds.x, bounds.y, bounds.w, 40);
    closeBounds = Rect(bounds.x + bounds.w - 40, bounds.y, 40, 40);
    contentBounds = Rect(bounds.x + 140, bounds.y + 40, bounds.w - 140, bounds.h - 40);
}

void Window::drawRect(const Rect& rect, const Color& color) {
    glUseProgram(shaderProg);
    glUniform2f(glGetUniformLocation(shaderProg, "uResolution"), screenW, screenH);
    glUniform2f(glGetUniformLocation(shaderProg, "uOffset"), rect.x, rect.y);
    glUniform2f(glGetUniformLocation(shaderProg, "uScale"), rect.w, rect.h);
    glUniform4f(glGetUniformLocation(shaderProg, "uColor"), color.r, color.g, color.b, color.a);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Window::drawBorder(const Rect& rect, const Color& color, float thickness) {
    Rect top(rect.x, rect.y, rect.w, thickness);
    Rect bottom(rect.x, rect.y + rect.h - thickness, rect.w, thickness);
    Rect left(rect.x, rect.y, thickness, rect.h);
    Rect right(rect.x + rect.w - thickness, rect.y, thickness, rect.h);
    drawRect(top, color);
    drawRect(bottom, color);
    drawRect(left, color);
    drawRect(right, color);
}

void Window::render() {
    if (!visible) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawRect(bounds, getThemeColor("bg"));
    drawBorder(bounds, getThemeColor("border"), 1.0f);
    drawRect(headerBounds, getThemeColor("headerActive"));

    if (config.closable) {
        drawRect(closeBounds, getThemeColor("button"));
    }

    Rect sidebar(bounds.x, bounds.y + 40, 140, bounds.h - 40);
    drawRect(sidebar, getThemeColor("content"));
    drawBorder(sidebar, getThemeColor("border"), 1.0f);

    float tabH = 40;
    for (size_t i = 0; i < tabs.size(); i++) {
        Rect tabRect(bounds.x, bounds.y + 40 + i * (tabH + 4), 140, tabH);
        bool isActive = (int)i == activeTab;
        drawRect(tabRect, isActive ? getThemeColor("tabActive") : getThemeColor("tabInactive"));

        if (isActive) {
            Rect indicator(tabRect.x + tabRect.w - 3, tabRect.y, 3, tabRect.h);
            drawRect(indicator, getThemeColor("accent"));
        }

        if (!tabs[i]->getIcon().empty()) {
            Rect iconRect(tabRect.x + 8, tabRect.y + 8, 24, 24);
            tabs[i]->drawIcon(iconRect, this);
        }
    }

    drawRect(contentBounds, getThemeColor("content"));

    if (activeTab >= 0 && activeTab < (int)tabs.size()) {
        tabs[activeTab]->render(contentBounds, this);
    }

    glDisable(GL_BLEND);
}

void Window::handleTouch(float x, float y, bool down) {
    if (!visible) return;

    if (down) {
        if (closeBounds.contains(x, y) && config.closable) {
            visible = false;
            return;
        }
        if (headerBounds.contains(x, y) && config.draggable) {
            dragging = true;
            dragOffsetX = x - bounds.x;
            dragOffsetY = y - bounds.y;
            return;
        }
        float tabH = 40;
        for (size_t i = 0; i < tabs.size(); i++) {
            Rect tabRect(bounds.x, bounds.y + 40 + i * (tabH + 4), 140, tabH);
            if (tabRect.contains(x, y)) {
                activeTab = i;
                return;
            }
        }
        if (activeTab >= 0 && activeTab < (int)tabs.size()) {
            tabs[activeTab]->handleTouch(x, y, down, contentBounds);
        }
    } else {
        dragging = false;
        if (activeTab >= 0 && activeTab < (int)tabs.size()) {
            tabs[activeTab]->handleTouch(x, y, down, contentBounds);
        }
    }

    if (dragging) {
        bounds.x = x - dragOffsetX;
        bounds.y = y - dragOffsetY;
        headerBounds.x = bounds.x;
        headerBounds.y = bounds.y;
        closeBounds.x = bounds.x + bounds.w - 40;
        closeBounds.y = bounds.y;
        contentBounds.x = bounds.x + 140;
        contentBounds.y = bounds.y + 40;
    }
}

void Window::addTab(Tab* tab) {
    tabs.push_back(tab);
}

void Window::setVisible(bool v) { visible = v; }
bool Window::isVisible() const { return visible; }
void Window::toggle() { visible = !visible; }

}
