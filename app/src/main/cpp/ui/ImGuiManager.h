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
private:
  bool showMenu = false;
  bool initialized = false;
  void setupStyle();
};
