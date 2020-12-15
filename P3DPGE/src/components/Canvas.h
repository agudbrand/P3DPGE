#pragma once
#include "Component.h"
#include "../utils/UI.h"

struct Canvas : public Component {
	std::vector<UI*> elements;
	bool hideAll;

	Canvas() {
		elements = std::vector<UI*>();
		hideAll = false;
	}

	Canvas(std::vector<UI*> elements, bool hideAll = false) {
		this->elements = elements;
		this->hideAll = hideAll;
	}

	~Canvas() {
		for(UI* el : elements) delete el;
		elements.clear();
	}
};