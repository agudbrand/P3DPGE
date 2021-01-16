#pragma once
#include "../internal/olcPixelGameEngine.h"
#include "../math/Math.h"
#include "../utils/Command.h"

//StringDecal height is 7 pixels and width is 8 pixels at a scale of 1 
#define FONT_HEIGHT		7
#define FONT_WIDTH		8
#define FONT_PADDING	6

struct EntityAdmin;

//TODO(delle) convert drawing to decals? (string and rect)
struct UI {
	Vector2 pos;
	Vector2 size;
	bool visible;
	EntityAdmin* admin;

	UI(EntityAdmin* admin, Vector2 pos, Vector2 size, bool visible) : admin(admin), pos(pos), size(size), visible(visible) {}

	virtual void Draw(olc::PixelGameEngine* p) = 0;
	virtual bool Clicked(Vector2 point) = 0;
	virtual void Update() {};
};

struct Button : public UI {
	Command* command;
	olc::Pixel bgColor;
	olc::Pixel borderColor;

	Button(EntityAdmin* admin, Vector2 pos, Vector2 size, Command* command = nullptr, bool visible = true,
			olc::Pixel bgColor = olc::GREY, olc::Pixel borderColor = olc::DARK_GREY)
	: UI(admin, pos, size, visible), command(command), bgColor(bgColor), borderColor(borderColor) {}

	bool Clicked(Vector2 point) override {
		if (Math::PointInRect(size, pos, point)) {
			if(command) { command->Exec(admin); }
			return true;
		}
		return false;
	}

	void Draw(olc::PixelGameEngine* p) override {
		if(visible) {
			p->FillRect(pos, size, bgColor);
			p->DrawRect(pos, size, borderColor);
		}
	}
};

struct TextButton : public Button {
	std::string text;
	std::string hoverText; //TODO(delle) implement hover text

	TextButton(EntityAdmin* admin, Vector2 pos, std::string text, Command* command = nullptr, std::string hoverText = "", Vector2 size = V2ZERO,
				bool visible = true, olc::Pixel bgColor = olc::GREY, olc::Pixel borderColor = olc::DARK_GREY)
	: Button(admin, pos, size, command, visible, bgColor, borderColor), text(text), hoverText(hoverText) {
		if(size == V2ZERO) {
			this->size = Vector2(FONT_WIDTH*text.size() + FONT_PADDING, FONT_HEIGHT + FONT_PADDING);
		}
	}

	void Draw(olc::PixelGameEngine* p) override {
		if(visible) {
			Button::Draw(p);
			Vector2 titlePos = Vector2(pos.x + (size.x/2 - FONT_WIDTH*text.size()/2), pos.y + FONT_PADDING/2);
			p->DrawString(titlePos, text);
		}
	}
};

struct TextField : public UI { //TODO(delle) implement TextField and TextBox
	std::string text;
	olc::Pixel borderColor;
	olc::Pixel fontColor;

	TextField(EntityAdmin* admin, Vector2 pos, Vector2 size, std::string text, bool visible = true, 
		olc::Pixel fontColor = olc::WHITE, olc::Pixel borderColor = olc::DARK_GREY)
	: UI(admin, pos, size, visible), fontColor(fontColor), borderColor(borderColor) {}

	bool Clicked(Vector2 point) override {
		if(!visible) { return false; }
		if (Math::PointInRect(size, pos, point)) {
			return true;
		}
		return false;
	}

	void Draw(olc::PixelGameEngine* p) override {
		if(visible) {
			p->DrawRect(pos, size, borderColor);
			p->DrawString(pos, text);
		}
	}
};

struct Panel : public UI {
	std::string title;
	std::vector<UI*> elements;
	olc::Pixel bgColor;
	olc::Pixel borderColor;
	float marginPadding;

	bool closed = true;
	Vector2 openSize;			//size of the panel when opened (including title)
	Vector2 titlePos;
	
	Panel(EntityAdmin* admin, Vector2 pos, std::string title, std::vector<UI*> elements, float marginPadding = 5, Vector2 size = V2ZERO, 
			bool visible = true, olc::Pixel bgColor = olc::GREY, olc::Pixel borderColor = olc::DARK_GREY)
	: UI(admin, pos, size, visible), title(title), elements(elements), marginPadding(marginPadding), bgColor(bgColor), 
	borderColor(borderColor) {
		if(size == V2ZERO) { this->size = Vector2(FONT_WIDTH*title.size() + FONT_PADDING, FONT_HEIGHT + FONT_PADDING); }
		Update();
	}

	void UpdateOpenSize() {
		float minWidth = 0;
		float minHeight = 0;
		for(UI* el : elements) {
			if(el->size.x > minWidth) { minWidth = el->size.x; }
			minHeight += el->size.y + FONT_PADDING;
		}

		float titleWidth = FONT_WIDTH*title.size() + FONT_PADDING;
		openSize.x = (titleWidth > minWidth) ? titleWidth + marginPadding*2 : minWidth + marginPadding*2;
		openSize.y = minHeight + marginPadding*2 - FONT_PADDING;
	}

	void UpdateElements() {
		titlePos = Vector2(pos.x + (size.x - FONT_WIDTH*title.size())/2, pos.y + FONT_PADDING);

		float rowY = pos.y + size.y + marginPadding;
		for(UI* el : elements) {
			el->pos.x = pos.x + marginPadding;
			el->pos.y = rowY;
			rowY = el->pos.y + el->size.y + FONT_PADDING;
		}
	}

	//TODO(delle) merge this into Clicked
	bool ClickedInTitle(Vector2 point) {
		if(!visible) { return false; }
		if (!closed && Math::PointInRect(size, pos, point)) { return true; }
		return false;
	}

	bool Clicked(Vector2 point) override {
		if(!visible) { return false; }
		if(Math::PointInRect(size, pos, point)) {
			closed = !closed;
			return true;
		}
		if(!closed) {
			for(UI* el : elements) {
				if(el->Clicked(point)) { break; }
			}
			return true;
		}
		return false;
	}

	void Draw(olc::PixelGameEngine* p) override {
		if(!visible) { return; }
		if(!closed){
			p->FillRect(pos + Vector2(0,size.y), openSize, bgColor);		//background
			p->DrawRect(pos + Vector2(0,size.y), openSize, borderColor);	//border

			for(UI* el : elements) {										//elements
				el->Draw(p);
			}
		}
		p->FillRect(pos, size, bgColor);								//title background
		p->DrawRect(pos, size, borderColor);							//title border
		p->DrawString(titlePos, title);									//title
	}

	void Update() {
		UpdateOpenSize();
		UpdateElements();
		for(UI* el : elements) { el->Update(); }
	}
};