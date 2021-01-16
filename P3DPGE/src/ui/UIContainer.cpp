#include "UIContainer.h"
#include "UI.h"

UIContainer::~UIContainer() {
	for(UI* ui : container) { delete ui; }
}

UIRow::UIRow(Vector2 position, UIAnchor anchor, bool growRight, float uniformWidth, float uniformHeight) {
	this->position = position;
	this->anchor = anchor;
	this->growRight = growRight;
	this->uniformHeight = uniformHeight;
	this->uniformWidth = uniformWidth;
}

void UIRow::Add(UI* element) {//TEST(delle) test if this degens by 1 pixel
	size_t size = container.size();
	if(growRight) {						//grows right
		if(uniformHeight) {					//uniform height
			if(uniformWidth) {					//uniform width
				element->size = Vector2(uniformWidth, uniformHeight);
				element->pos = Vector2(position.x + uniformWidth * size, position.y);
			} else {							//not uniform width
				element->size = Vector2(element->size.x, uniformHeight);
				element->pos = (size > 0) ? Vector2(container.back()->pos.x + container.back()->size.x, position.y) : position; 
			}
		} else {							//not uniform height
			if(uniformWidth) {					//uniform width
				element->size = Vector2(uniformWidth, element->size.y);
				element->pos = Vector2(position.x + uniformWidth * container.size(), position.y);
			} else {							//not uniform width
				element->pos = (size > 0) ? Vector2(container.back()->pos.x + container.back()->size.x, position.y) : position;
			}
		}
	} else {							//grows left
		if(uniformHeight) {					//uniform height
			if(uniformWidth) {					//uniform width
				element->size = Vector2(uniformWidth, uniformHeight);
				element->pos = Vector2(position.x - uniformWidth * (container.size()+1), position.y);
			} else {							//not uniform width
				element->size = Vector2(element->size.x, uniformHeight);
				element->pos = (size > 0) ? Vector2(container.back()->pos.x - element->size.x, position.y) : Vector2(position.x - element->size.x, position.y);
			}
		} else {							//not uniform height
			if(uniformWidth) {					//uniform width
				element->size = Vector2(uniformWidth, element->size.y);
				element->pos = Vector2(position.x - uniformWidth * (container.size()+1), position.y);
			} else {							//not uniform width
				element->pos = (size > 0) ? Vector2(container.back()->pos.x - element->size.x, position.y) : Vector2(position.x - element->size.x, position.y);;
			}
		}
	}
	element->Update();
	container.push_back(element);
}