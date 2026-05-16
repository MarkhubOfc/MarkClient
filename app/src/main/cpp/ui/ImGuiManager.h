#pragma once
#include <android/native_window.h>
#include <GLES3/gl3.h>

class ImGuiManager {
public:
  void init(ANativeWindow* window, int width, int height);
  void shutdown();
  void newFrame();
  void render();
  void renderClickGUI();
  bool isVisible() { return showMenu; }
  void toggle() { showMenu = !showMenu; }
  void show() { showMenu = true; }
  void hide() { showMenu = false; }
private:
  bool showMenu = true;
  bool initialized = false;
  void setupStyle();
};
