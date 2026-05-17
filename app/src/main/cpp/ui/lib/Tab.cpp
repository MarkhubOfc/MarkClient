#include "Tab.h"
#include "Window.h"
#include "TextureManager.h"

namespace LoadLib {

Tab::Tab(const TabConfig& cfg) : config(cfg) {
    if (!config.icon.empty()) {
        iconTexture = TextureManager::get().loadTexture(config.icon);
    }
}

void Tab::addButton(const Button& btn) {
    buttons.push_back(btn);
}

void Tab::addToggle(const Toggle& toggle) {
    toggles.push_back(toggle);
}

void Tab::addSlider(const Slider& slider) {
    sliders.push_back(slider);
}

void Tab::drawIcon(const Rect& rect, Window* window) {
    if (!iconTexture || !iconTexture->id) return;
    TextureManager::get().bindTexture(iconTexture->id, 0);

    static const char* vs = R"(
        #version 300 es
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aTex;
        out vec2 vTex;
        uniform vec2 uResolution;
        uniform vec2 uOffset;
        uniform vec2 uScale;
        void main() {
            vec2 pos = (aPos * uScale + uOffset) / uResolution * 2.0 - 1.0;
            gl_Position = vec4(pos.x, -pos.y, 0.0, 1.0);
            vTex = aTex;
        }
    )";

    static const char* fs = R"(
        #version 300 es
        precision mediump float;
        in vec2 vTex;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        void main() {
            fragColor = texture(uTexture, vTex);
        }
    )";

    static GLuint prog = 0;
    static GLuint vao = 0;
    static GLuint vbo = 0;

    if (!prog) {
        GLuint v = glCreateShader(GL_VERTEX_SHADER);
        GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(v, 1, &vs, nullptr);
        glShaderSource(f, 1, &fs, nullptr);
        glCompileShader(v);
        glCompileShader(f);
        prog = glCreateProgram();
        glAttachShader(prog, v);
        glAttachShader(prog, f);
        glLinkProgram(prog);
        glDeleteShader(v);
        glDeleteShader(f);

        float verts[] = {0,0, 0,1, 1,0, 1,1, 0,1, 0,0, 1,1, 1,0};
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    glUseProgram(prog);
    glUniform2f(glGetUniformLocation(prog, "uResolution"), window->getScreenW(), window->getScreenH());
    glUniform2f(glGetUniformLocation(prog, "uOffset"), rect.x, rect.y);
    glUniform2f(glGetUniformLocation(prog, "uScale"), rect.w, rect.h);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Tab::render(const Rect& content, Window* window) {
    float pad = 12;
    float itemH = 50;
    float colW = (content.w - pad * 4) / 3.0f;
    if (colW < 100) colW = 100;

    float cx = content.x + pad;
    float cy = content.y + pad;
    int col = 0;

    for (const auto& btn : buttons) {
        Rect r(cx, cy, colW, itemH);
        window->drawRect(r, window->getThemeColor("button"));
        window->drawBorder(r, window->getThemeColor("border"), 1.0f);
        cx += colW + pad;
        col++;
        if (col >= 3) { col = 0; cx = content.x + pad; cy += itemH + pad; }
    }

    if (col > 0) { cy += itemH + pad; }
    cx = content.x + pad;

    for (const auto& toggle : toggles) {
        Rect r(cx, cy, content.w - pad * 2, itemH);
        bool val = toggle.value ? *toggle.value : false;
        window->drawRect(r, window->getThemeColor("content"));
        Rect knob(r.x + r.w - 50, r.y + 5, 40, r.h - 10);
        window->drawRect(knob, val ? window->getThemeColor("tabActive") : window->getThemeColor("button"));
        window->drawBorder(knob, window->getThemeColor("border"), 1.0f);
        cy += itemH + pad;
    }

    for (const auto& slider : sliders) {
        Rect r(cx, cy, content.w - pad * 2, itemH);
        float pct = (slider.value ? *slider.value : slider.min) / (slider.max - slider.min);
        if (pct < 0) pct = 0; if (pct > 1) pct = 1;
        Rect track(r.x, r.y + r.h * 0.4f, r.w, 8);
        Rect fill(r.x, track.y, track.w * pct, track.h);
        window->drawRect(track, window->getThemeColor("button"));
        window->drawRect(fill, window->getThemeColor("tabActive"));
        cy += itemH + pad;
    }
}

void Tab::handleTouch(float x, float y, bool down, const Rect& content) {
    if (!down) return;
    float pad = 12;
    float itemH = 50;
    float colW = (content.w - pad * 4) / 3.0f;
    float cx = content.x + pad;
    float cy = content.y + pad;
    int col = 0;

    for (auto& btn : buttons) {
        Rect r(cx, cy, colW, itemH);
        if (r.contains(x, y) && btn.enabled && btn.onClick) { btn.onClick(); return; }
        cx += colW + pad;
        col++;
        if (col >= 3) { col = 0; cx = content.x + pad; cy += itemH + pad; }
    }

    if (col > 0) { cy += itemH + pad; }
    cx = content.x + pad;

    for (auto& toggle : toggles) {
        Rect r(cx, cy, content.w - pad * 2, itemH);
        if (r.contains(x, y) && toggle.value) {
            *toggle.value = !*toggle.value;
            if (toggle.onChange) toggle.onChange(*toggle.value);
            return;
        }
        cy += itemH + pad;
    }

    for (auto& slider : sliders) {
        Rect r(cx, cy, content.w - pad * 2, itemH);
        if (r.contains(x, y)) {
            float pct = (x - r.x) / r.w;
            if (pct < 0) pct = 0; if (pct > 1) pct = 1;
            float newVal = slider.min + pct * (slider.max - slider.min);
            if (slider.value) *slider.value = newVal;
            if (slider.onChange) slider.onChange(newVal);
            return;
        }
        cy += itemH + pad;
    }
}

}
