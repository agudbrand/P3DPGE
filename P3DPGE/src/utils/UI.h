#pragma once
#include "../internal/olcPixelGameEngine.h"
#include "../math/Math.h"
#include "Command.h"

#define FONT_HEIGHT	7
#define FONT_WIDTH	8

struct EntityAdmin;

//temporary UI class placement
//I have no idea if I want UI to be a child of Entity or not yet so for now it will stay
//as its own class.
//StringDecal height is 7 pixels and width is 8 pixels at a scale of 1 

struct UI {
	Vector2 pos;
	Vector2 size;
	olc::Pixel color;

	bool visible;
	EntityAdmin* admin;

	UI() {}
	UI(EntityAdmin* admin, Vector2 pos, Vector2 size, olc::Pixel color = olc::WHITE, bool visible = true){
		this->admin = admin;
		this->pos = pos;
		this->size = size;
		this->color = color;
		this->visible = visible;
	}

	virtual void Draw(olc::PixelGameEngine* p) = 0;
	virtual bool Clicked(Vector2 point) { return false; };

	virtual std::string str() {
		std::string s = "pos: " + pos.str() + "\n" +
						"size " + size.str() + "\n";
		return s;
	}

	virtual std::string titlestr() {
		return "base ui";
	}
};

struct Button : public UI {
	Command* command;

	std::string title;
	std::string description;

	bool holds_menu = false;
	int sub_menu_id; //maybe come up with a way to avoid having to define this 

	//pos is defined as the upper left corner of buttons for right now unless
	//I change it later idk yet
	Button(EntityAdmin* admin, Vector2 pos, Vector2 size, Command* command, std::string title = "", std::string description = "", olc::Pixel color = olc::WHITE) : UI(admin, pos, size, color) {
		this->command = command;
		this->title = title;
		this->description = description;
	}

	//for buttons to be put in a menu
	//could make it so buttons determine their own color on a menu
	Button(EntityAdmin* admin, Command* command, std::string title = "", std::string description = "", bool holds_menu = false, int sub_menu_id = 0, olc::Pixel color = olc::WHITE) {
		this->admin = admin;
		this->command = command;
		this->title = title;
		this->description = description;
		this->holds_menu = holds_menu;
		this->sub_menu_id = sub_menu_id;
	}

	void Draw(olc::PixelGameEngine* p) override {
		p->FillRect(pos, size, color);
	}

	bool Clicked(Vector2 point) override {
		if (Math::PointInRect(size, pos, point)) {
			if(command) { command->Exec(admin); }
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

struct Menu : public UI {
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

	int str_max_width = 0;

	int row_h = 15;
	int border_room = 10;
	int height_room = 10;

	Menu(EntityAdmin* admin, Vector2 pos, std::string title, std::string cl_title, std::vector<Button*> buttons, 
		std::vector<Menu*> sub_menus = std::vector<Menu*>(), olc::Pixel color = olc::WHITE) : UI(admin, pos, V2ZERO, color) {
		this->title = title;
		this->cl_title = cl_title;
		this->buttons = buttons;
		calc_min_width();
		
		resize();
		
		this->sub_menus = sub_menus;
		cl_button = new Button(admin, 0, "X");
	}

	void calc_min_width() {
		int bt_width = 0;
		int bd_width = 0;
		int b_min_width = 0;

		for (Button* b : buttons) {
			bt_width = FONT_WIDTH * b->title.size();
			bd_width = FONT_WIDTH * b->description.size();
			b_min_width = bt_width + bd_width + 10;
			if (b_min_width > min_width) { min_width = b_min_width; }
			if (bt_width > title_max_width) { title_max_width = bt_width; }
			if (bd_width > descr_max_width) { descr_max_width = bd_width; }
		} 

		Debug::ToString(1, title + std::string(": ") + std::to_string(min_width));

		for (auto& s : dynamic_strings.container) {
			if (s && s.value().size() * FONT_WIDTH > str_max_width) {
				str_max_width = s.value().size() * FONT_WIDTH; 
				min_width = bt_width + bd_width + str_max_width;
			}
		}

		if (descr_max_width == 0) {
			no_desc = true;
			min_width += str_max_width;
		} else {
			min_width = bt_width + bd_width + str_max_width;
		}

		Debug::ToString(1, title + std::string(": ") + std::to_string(min_width));
	}

	void resize() {

		if (buttons.size() == 0 && dynamic_strings.real_size == 0) {
			size = Vector2(title.size() * FONT_WIDTH + 35, 20);
		}
		else {
			calc_min_width();
			Debug::ToString(0, std::to_string(min_width));
			size.x = min_width + 2 * border_room;
			Debug::ToString(0, std::to_string(size.x));
			size.y = row_h * (buttons.size() + dynamic_strings.real_size) + row_h + height_room;
			if (size.x < title.size() * FONT_WIDTH + 35) {
				size.x = title.size() * FONT_WIDTH + 35;
			}
			Debug::ToString(0, std::to_string(size.x));
		}

		Debug::ToString(0, title + std::string(": ") + size.str());
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
			p->FillRect(Vector2(pos.x - 1, pos.y - 2), Vector2(cl_title.size() * FONT_WIDTH + 4, 9), olc::VERY_DARK_CYAN);
			cl_button->size = Vector2(cl_title.size() * FONT_WIDTH + 4, 9);
			cl_button->pos = Vector2(pos.x - 1, pos.y - 2);
			p->DrawString(pos, cl_title);
		}
		else {
			//background
			p->FillRect(pos, size, olc::Pixel(85, 55, 57));

			//title background
			p->FillRect(pos, Vector2(size.x, row_h), olc::VERY_DARK_CYAN);

			//title
			p->DrawString(Vector2(pos.x + ((size.x / 2) - (FONT_WIDTH * title.size() / 2)), pos.y + (row_h / 2) - 4), title);

			int bspaceinc = (size.y - row_h) / (buttons.size() + dynamic_strings.real_size);

			//if sub_menu, cl_button is null
			if (cl_button) cl_button->pos = Vector2(pos.x + size.x - 14, pos.y - 5 + row_h / 2);
			if (cl_button) cl_button->size = Vector2(10, 10);

			if (cl_button) p->FillRect(Vector2(cl_button->pos.x - 2, cl_button->pos.y - 2), Vector2(10, 10), olc::Pixel(150, 57, 46));
			if (cl_button) p->DrawString(cl_button->pos, cl_button->title);

			//buttons
			for (Button* b : buttons) {
				p->FillRect(Vector2(pos.x + border_room - 2, pos.y + row_h - 2 + ((index * bspaceinc) - (bspaceinc * 0.75f))), Vector2(b->title.size() * FONT_WIDTH + 4, bspaceinc - 2), olc::Pixel(0, 31, 31));
				b->pos = Vector2(pos.x + border_room, pos.y + row_h - 3 + ((index * bspaceinc) - (bspaceinc * 0.75f))); // could probably do this somewhere else.
				b->size = Vector2(b->title.size() * FONT_WIDTH + 4, bspaceinc - 2);
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

	bool Clicked(Vector2 point) override {
		if (disp_sub_menu) {
			if (!sub_menus[sub_disp]->Clicked(point)) {
				disp_sub_menu = false;
			}
			else {
				return true;
			}
		}
		if (Math::PointInRect(size, pos, point)) {
			if (cl_button && cl_button->Clicked(point)) {
				closed = !closed;
			}
			for (Button* b : buttons) {
				if (b->Clicked(point)) {
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