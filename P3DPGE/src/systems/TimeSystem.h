#pragma once
#include "System.h"

struct TimeSystem : public System {
	void Init() override;
	void Update() override;
};