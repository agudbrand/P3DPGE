#pragma once
#include "Component.h"

struct UIContainer;
struct Canvas : public Component {
	std::vector<UIContainer*> containers;
	bool hideAll;

	Canvas() {
		containers = std::vector<UIContainer*>();
		hideAll = false;
	}

	Canvas(std::vector<UIContainer*> containers, bool hideAll = false) {
		this->containers = containers;
		this->hideAll = hideAll;
	}

	~Canvas() {
		for(UIContainer* con : containers) delete con;
		containers.clear();
	}
};