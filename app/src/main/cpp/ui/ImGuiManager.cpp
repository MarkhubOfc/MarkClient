#include "ImGuiManager.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_android.h"
#include "../Client.h"
#include "../modules/ModuleManager.h"
#include "../utils/Logger.h"

void ImGuiManager::init(EGLDisplay display, EGLSurface surface, int width, int height) {
  if (initialized) return;
  
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2((float)width, (float)height);
  
  ImGui_ImplOpenGL3_Init("#version 300 es");
  ImGui_ImplAndroid_Init(surface);
  
  setupStyle();
  initialized = true;
  Logger::info("ImGui initialized");
}

void ImGuiManager::shutdown() {
  if (!initialized) return;
  
  ImGui_ImplAndroid_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();
  initialized = false;
}

void ImGuiManager::newFrame() {
  if (!initialized) return;
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplAndroid_NewFrame();
  ImGui::NewFrame();
}

void ImGuiManager::render() {
  if (!initialized) return;
  if (showMenu) {
    renderClickGUI();
  }
  
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::renderClickGUI() {
  ImGuiIO& io = ImGui::GetIO();
  ImVec2 windowSize(800, 500);
  ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f, (io.DisplaySize.y - windowSize.y) * 0.5f);
  
  ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver);
  ImGui::Begin("Mark Client v1.0.0", &showMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
  
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
  
  static int selectedCategory = 0;
  const char* categories[] = {"Combat", "Movement", "Render", "World", "Misc"};
  const int categoryCount = 5;
  
  ImGui::BeginChild("CategorySidebar", ImVec2(140, -ImGui::GetFrameHeightWithSpacing() - 10), true);
  for (int i = 0; i < categoryCount; i++) {
    if (ImGui::Selectable(categories[i], selectedCategory == i, 0, ImVec2(0, 35))) {
      selectedCategory = i;
    }
  }
  ImGui::EndChild();
  
  ImGui::SameLine();
  
  ImGui::BeginChild("ModulesArea", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() - 10), true);
  
  auto modules = Client::get().getModuleManager()->getModulesByCategory((Category)selectedCategory);
  
  if (modules.empty()) {
    ImGui::TextDisabled("No modules in this category");
  } else {
    for (size_t i = 0; i < modules.size(); i++) {
      auto* mod = modules[i];
      
      bool enabled = mod->isEnabled();
      ImVec4 color = enabled ? ImVec4(0.0f, 0.47f, 0.84f, 1.0f) : ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
      
      ImGui::PushStyleColor(ImGuiCol_Button, color);
      ImVec4 hoverColor = enabled ? ImVec4(1.0f, 0.84f, 0.0f, 0.8f) : ImVec4(0.0f, 0.58f, 1.0f, 0.6f);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
      
      if (ImGui::Button(mod->getName().c_str(), ImVec2(160, 45))) {
        mod->toggle();
      }
      
      ImGui::PopStyleColor(2);
      
      if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", mod->getDescription().c_str());
      }
      
      if ((i + 1) % 3 != 0 && i != modules.size() - 1) {
        ImGui::SameLine();
      }
    }
  }
  
  ImGui::EndChild();
  
  ImGui::Separator();
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.84f, 0.0f, 1.0f));
  ImGui::Text("Modules: %zu", modules.size());
  ImGui::PopStyleColor();
  
  ImGui::SameLine(ImGui::GetWindowWidth() - 100);
  if (ImGui::SmallButton("Close##menu")) {
    showMenu = false;
  }
  
  ImGui::PopStyleVar();
  ImGui::End();
}

void ImGuiManager::setupStyle() {
  ImGuiStyle& style = ImGui::GetStyle();
  
  style.WindowPadding = ImVec2(12, 12);
  style.FramePadding = ImVec2(8, 6);
  style.ItemSpacing = ImVec2(8, 8);
  style.ItemInnerSpacing = ImVec2(6, 4);
  style.IndentSpacing = 20.0f;
  
  style.WindowRounding = 0.0f;
  style.FrameRounding = 2.0f;
  style.PopupRounding = 2.0f;
  style.ScrollbarRounding = 2.0f;
  style.GrabRounding = 2.0f;
  
  style.WindowBorderSize = 1.0f;
  style.FrameBorderSize = 1.0f;
  
  ImVec4 black = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  ImVec4 darkGray = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
  ImVec4 blue = ImVec4(0.00f, 0.47f, 0.84f, 1.00f);
  ImVec4 yellow = ImVec4(1.00f, 0.84f, 0.00f, 1.00f);
  
  style.Colors[ImGuiCol_WindowBg] = black;
  style.Colors[ImGuiCol_ChildBg] = darkGray;
  style.Colors[ImGuiCol_PopupBg] = black;
  
  style.Colors[ImGuiCol_Border] = blue;
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  
  style.Colors[ImGuiCol_FrameBg] = darkGray;
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
  style.Colors[ImGuiCol_FrameBgActive] = blue;
  
  style.Colors[ImGuiCol_TitleBg] = black;
  style.Colors[ImGuiCol_TitleBgActive] = blue;
  style.Colors[ImGuiCol_TitleBgCollapsed] = darkGray;
  
  style.Colors[ImGuiCol_Button] = blue;
  style.Colors[ImGuiCol_ButtonHovered] = yellow;
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.9f, 0.0f, 1.0f);
  
  style.Colors[ImGuiCol_Header] = blue;
  style.Colors[ImGuiCol_HeaderHovered] = yellow;
  style.Colors[ImGuiCol_HeaderActive] = yellow;
  
  style.Colors[ImGuiCol_Separator] = blue;
  style.Colors[ImGuiCol_SeparatorHovered] = yellow;
  style.Colors[ImGuiCol_SeparatorActive] = yellow;
  
  style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
  
  style.Colors[ImGuiCol_CheckMark] = yellow;
  style.Colors[ImGuiCol_SliderGrab] = yellow;
  style.Colors[ImGuiCol_SliderGrabActive] = yellow;
  
  style.Colors[ImGuiCol_ScrollbarBg] = black;
  style.Colors[ImGuiCol_ScrollbarGrab] = blue;
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = yellow;
  style.Colors[ImGuiCol_ScrollbarGrabActive] = yellow;
}
