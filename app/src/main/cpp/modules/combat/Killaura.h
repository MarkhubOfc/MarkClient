#pragma once
#include "../Module.h"

class Killaura : public Module {
public:
  Killaura();
  void onEnable() override;
  void onTick() override;
};
