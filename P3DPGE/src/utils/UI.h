#pragma once
#include "../internal/olcPixelGameEngine.h"
#include "../math/Math.h"

typedef void (*Action)(olc::PixelGameEngine* p);

//temporary UI class placement
//I have no idea if I want UI to be a child of Entity or not yet so for now it will stay
//as its own class.
//StringDecal height is 7 pixels and width is 8 pixels at a scale of 1 

class UI {
public:
	Vector2 pos;
	Vector2 size;
	olc::Pixel color;
	bool visible;

	UI() {}
	UI(Vector2 pos, Vector2 size, olc::Pixel color = olc::WHITE, bool visible = true){
		this->pos = pos;
		this->size = size;
		this->color = color;
		this->visible = visible;
	}

	virtual void Draw(olc::PixelGameEngine* p) = 0;
	virtual bool Clicked(Vector2 point, olc::PixelGameEngine* p) { return false; };

	virtual std::string str() {
		std::string s = "pos: " + pos.str() + "\n" +
						"size " + size.str() + "\n";
		return s;
	}

	virtual std::string titlestr() {
		return "base ui";
	}
};

class Button : public UI {
public:

	Vector2 size;
	Action action;

	std::string title;
	std::string description;

	bool holds_menu = false;
	int sub_menu_id; //maybe come up with a way to avoid having to define this 

	//pos is defined as the upper left corner of buttons for right now unless
	//I change it later idk yet
	Button(Vector2 pos, Vector2 size, Action action, std::string title = "", std::string description = "", olc::Pixel color = olc::WHITE) : UI(pos, size, color) {
		this->action = action;
		this->title = title;
		this->description = description;
	}

	//for buttons to be put in a menu
	//could make it so buttons determine their own color on a menu
	Button(Action action, std::string title = "", std::string description = "", bool holds_menu = false, int sub_menu_id = 0, olc::Pixel color = olc::WHITE) {
		this->action = action;
		this->title = title;
		this->description = description;
		this->holds_menu = holds_menu;
		this->sub_menu_id = sub_menu_id;
	}

	void Draw(olc::PixelGameEngine* p) override {
		p->FillRect(pos, size, color);
	}

	bool Clicked(Vector2 point, olc::PixelGameEngine* p) override {
		if (Math::PointInRect(size, pos, point)) {
			action(p);
			return true;
		}
		return false;
	}

	std::string str() override {
		std::string s = UI::str() + "\n" +
						"title: " + title;
		return s;
	}
	
	std::string titlestr() override {
		return title;
	}
};



//Menu layout:
//
//		|-----------------------------------|
//		|				title				|
//		|-----------------------------------|
//		|Button 1 | Button 1 description    |
//		|Button 2 | Button 2 description	|
//		|...      |	...						|
//		|...      |	...						|
//		|...      |	...						|
//		|...      |	...						|
//		|...      |	...						|
//		|-----------------------------------|
//		width of the menu is determined by either inputted size
//		or min width calculated from how many characters there are
//
//
//		currently until i figure out how to place things
//		nicely

class Menu : public UI {
public:
	std::vector<Button*> buttons;
	std::string title;

	ContainerManager<std::string> dynamic_strings;

	Button* cl_button;
	bool closed = true;
	std::string cl_title;

	std::vector<Menu*> sub_menus;
	bool disp_sub_menu = false;
	int sub_disp;

	int min_width;

	int title_max_width;
	int descr_max_width;
	bool no_desc = false;

	int str_max_width;

	int row_h = 15;
	int border_room = 10;
	int height_room = 10;

	Menu(Vector2 pos, std::string title, std::string cl_title, std::vector<Button*> buttons, 
		std::vector<Menu*> sub_menus = std::vector<Menu*>(), olc::Pixel color = olc::WHITE) : UI(pos, V2ZERO, color) {
		this->title = title;
		this->cl_title = cl_title;
		this->buttons = buttons;
		calc_min_width();
		
		resize();
		
		this->sub_menus = sub_menus;
		cl_button = new Button(([](olc::PixelGameEngine* p) {}), "X");
	}

	void calc_min_width() {
		int bt_width = 0;
		int bd_width = 0;
		int b_min_width = 0;

		for (Button* b : buttons) {
			bt_width = 8 * b->title.size();
			bd_width = 8 * b->description.size();
			b_min_width = bt_width + bd_width + 10;
			if (b_min_width > min_width) { min_width = b_min_width; }
			if (bt_width > title_max_width) { title_max_width = bt_width; }
			if (bd_width > descr_max_width) { descr_max_width = bd_width; }
		} 

		for (auto& s : dynamic_strings.container) {
			if (s) {
				if (s.value().size() * 8 > str_max_width) { str_max_width = s.value().size() * 8; }
			}
			min_width = bt_width + bd_width + str_max_width;
		}

		if (descr_max_width == 0) {
			no_desc = true;
			min_width = bt_width + bd_width + str_max_width;
			
		}

	}

	void resize() {

		if (buttons.size() == 0 && dynamic_strings.real_size == 0) {
			size = Vector2(title.size() * 8 + 35, 20);
		}
		else {
			calc_min_width();
			size.x = min_width + 2 * border_room;
			size.y = row_h * (buttons.size() + dynamic_strings.real_size) + row_h + height_room;
			if (size.x < title.size() * 8 + 35) {
				size.x = title.size() * 8 + 35;
			}
		}
	}

	void add_button(Button* b) {
		buttons.push_back(b);
	}

	void update_dyn_strings(ContainerManager<std::string> cm) {
		if (dynamic_strings.real_size != cm.real_size) {
			dynamic_strings = cm;
			resize();
		}
		else {
			dynamic_strings = cm;
		}
	}

	void Draw(olc::PixelGameEngine* p) override {

		int index = 1;

		if (closed) {
			p->FillRect(Vector2(pos.x - 1, pos.y - 2), Vector2(cl_title.size() * 8 + 4, 9), olc::VERY_DARK_CYAN);
			cl_button->size = Vector2(cl_title.size() * 8 + 4, 9);
			cl_button->pos = Vector2(pos.x - 1, pos.y - 2);
			p->DrawString(pos, cl_title);
		}
		else {
			//background
			p->FillRect(pos, size, olc::Pixel(85, 55, 57));

			//title background
			p->FillRect(pos, Vector2(size.x, row_h), olc::VERY_DARK_CYAN);

			//title
			p->DrawString(Vector2(pos.x + ((size.x / 2) - (8 * title.size() / 2)), pos.y + (row_h / 2) - 4), title);

			int bspaceinc = (size.y - row_h) / (buttons.size() + dynamic_strings.real_size);

			//if sub_menu, cl_button is null
			if (cl_button) cl_button->pos = Vector2(pos.x + size.x - 14, pos.y - 5 + row_h / 2);
			if (cl_button) cl_button->size = Vector2(10, 10);

			if (cl_button) p->FillRect(Vector2(cl_button->pos.x - 2, cl_button->pos.y - 2), Vector2(10, 10), olc::Pixel(150, 57, 46));
			if (cl_button) p->DrawString(cl_button->pos, cl_button->title);

			//buttons
			for (Button* b : buttons) {
				p->FillRect(Vector2(pos.x + border_room - 2, pos.y + row_h - 2 + ((index * bspaceinc) - (bspaceinc * 0.75f))), Vector2(b->title.size() * 8 + 4, bspaceinc - 2), olc::Pixel(0, 31, 31));
				b->pos = Vector2(pos.x + border_room, pos.y + row_h - 3 + ((index * bspaceinc) - (bspaceinc * 0.75f))); // could probably do this somewhere else.
				b->size = Vector2(b->title.size() * 8 + 4, bspaceinc - 2);
				p->DrawString(Vector2(pos.x + border_room, pos.y - 4 + row_h + ((index * bspaceinc) - (bspaceinc / 2))), b->title);
				p->DrawString(Vector2(pos.x + border_room + title_max_width + 10, pos.y - 4 + row_h + ((index * bspaceinc) - (bspaceinc / 2))), b->description);
				index++;
			}
			
			//border
			p->DrawRect(pos, size, olc::DARK_CYAN);

			//separators 
			p->FillRect(Vector2(pos.x, pos.y + (row_h - (0.2 * row_h))), Vector2(size.x, 3), olc::DARK_CYAN);
			if(!no_desc) p->FillRect(Vector2(pos.x + title_max_width + 14, pos.y + row_h), Vector2(3, size.y - row_h - (row_h * dynamic_strings.real_size)), olc::DARK_CYAN);
			
			//draw changing strings
			if (dynamic_strings.real_size != 0) {
				for (auto& s : dynamic_strings.container) {
					if (s.has_value()) {
						std::string str = s.value();
						p->DrawString(Vector2(pos.x + border_room, pos.y - 4 + row_h + ((index * bspaceinc) - (bspaceinc / 2))), str);
						index++;
					}
				}
				resize();
			}

			if (disp_sub_menu) {
				if (sub_menus[sub_disp]->cl_button) { sub_menus[sub_disp]->cl_button = nullptr; }
				if (sub_menus[sub_disp]->closed) { sub_menus[sub_disp]->closed = false; }
				sub_menus[sub_disp]->Draw(p);
			}
		}
	}

	bool ClickedInTitle(Vector2 point) {
		if (!closed && Math::PointInRect(Vector2(size.x, row_h), pos, point)) {
			return true;
		}
		return false;
	}

	bool Clicked(Vector2 point, olc::PixelGameEngine* p) override {

		if (disp_sub_menu) {
			if (!sub_menus[sub_disp]->Clicked(point, p)) {
				disp_sub_menu = false;
			}
			else {
				return true;
			}
		}
		if (Math::PointInRect(size, pos, point)) {
			if (cl_button && cl_button->Clicked(point, p)) {
				closed = !closed;
			}
			for (Button* b : buttons) {
				if (b->Clicked(point, p)) {
					if (b->holds_menu) {
						disp_sub_menu = true;
						sub_disp = b->sub_menu_id;
						sub_menus[sub_disp]->pos = point;
					}
					return true;
				}
			}
			return true;
		}
		return false;
	}

	std::string str() override {
		std::string s = UI::str() + "\n" +
						"title: " + title;
		return s;
	}

	std::string titlestr() override {
		return title;
	}

};