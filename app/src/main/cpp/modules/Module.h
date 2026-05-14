#pragma once
#include <string>

enum class Category { COMBAT, MOVEMENT, RENDER, WORLD, MISC };

class Module {
public:
  Module(std::string n, std::string d, Category c)
    : name(std::move(n)), description(std::move(d)), category(c) {}
  virtual ~Module() = default;
  virtual void onEnable() {}
  virtual void onDisable() {}
  virtual void onTick() {}
  
  void toggle() {
    enabled = !enabled;
    enabled ? onEnable() : onDisable();
  }
  
  const std::string& getName() const { return name; }
  bool isEnabled() const { return enabled; }
  Category getCategory() const { return category; }
  
protected:
  std::string name;
  std::string description;
  Category category;
  bool enabled = false;
};
