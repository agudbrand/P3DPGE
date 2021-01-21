#include "ConsoleSystem.h"

#include "../components/Console.h"
#include "../utils/Command.h"
#include "../internal/imgui/imgui_impl_pge.h"
#include "../internal/imgui/imgui_impl_opengl2.h"

#define RegColorFormat std::regex("(?:\\[c:([^\\]]*)\\]([^\\]]*)\\[c\\]|([^\\[]+))")

using namespace ImGui;

EntityAdmin* locadmin; //so I can access admin in TextEditCallback
Console* loccon; //so I can access console in TextEditCallback

std::map<std::string, olc::Pixel> colstrmap{ //TODO(, sushi) extend this map
	{"red", olc::RED},
	{"blue", olc::BLUE},
	{"cyan", olc::CYAN},
	{"green", olc::GREEN},
	{"yellow", olc::YELLOW},
	{"none", olc::BLANK} //for unformatted text
};

void AddLog(std::string input, Console* c) {
	c->buffer.push_back(input);


}

void ClearLog(Console* c) {
	c->buffer.clear();
}

std::string ExecCommand(std::string command, std::string args, EntityAdmin* admin) {
	if (admin->commands.find(command) != admin->commands.end()) {
		return admin->commands.at(command)->Exec(admin, args);
	}
	else {
		return "Command \"" + command + "\" not found.";
	}
}

int TextEditCallback(ImGuiInputTextCallbackData* data) {
	switch (data->EventFlag) {
	case ImGuiInputTextFlags_CallbackCompletion:{
			std::string input = data->Buf;
			std::regex e(input + ".*");
			std::vector<std::string> posi;
			for (std::pair<std::string, Command*> c : locadmin->commands) {
				if (std::regex_search(c.first, e)) {
					posi.push_back(c.first);
				}
			}
			//TODO(, sushi) implement showing a commands help if tab is pressed when the command is already typed

			if (posi.size() == 0) {
				AddLog("no matches found", loccon);
			}
			else if (posi.size() == 1) {
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(data->CursorPos, posi[0].c_str());
			}
			else {
				AddLog("possible matches:\n", loccon);
				for (std::string s : posi) {
					AddLog(s, loccon);
				}

				int charmatched = 0;
				bool match = true;
				while (!match) {
					for (int i = 0; i < posi.size(); i++) {
						if (i == posi.size() - 1) {
							//match = match && (posi[i][i])
						}
					}
				}
			}

			break;
		}
	case ImGuiInputTextFlags_CallbackHistory:{
		
		const int prev_hist_pos = loccon->historyPos;
		if (data->EventKey == ImGuiKey_UpArrow) {
			if (loccon->historyPos == -1) {
				loccon->historyPos = loccon->history.size() - 1;
			}
			else if (loccon->historyPos > 0){
				loccon->historyPos--;
			}
		}
		else if (data->EventKey == ImGuiKey_DownArrow){
			if (loccon->historyPos != -1) {
				if (++loccon->historyPos >= loccon->history.size()) {
					loccon->historyPos = -1;
				}
			}
		}

		if (prev_hist_pos != loccon->historyPos)
		{
			std::string history_str = (loccon->historyPos >= 0) ? loccon->history[loccon->historyPos] : "";
			data->DeleteChars(0, data->BufTextLen);
			data->InsertChars(0, history_str.c_str());
		}
		
		break;
	}		
	}
	

	return 0;
}

ImVec4 olcPixToVec4(olc::Pixel p) {
	return ImVec4((float)p.r / 255, (float)p.g / 255, (float)p.b / 255, p.a / 255);
}

static int TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
	return TextEditCallback(data); //i dont know why we do this
}

void ConsoleSystem::DrawConsole() {

	Console* c = admin->console;

	SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Console", 0, ImGuiWindowFlags_MenuBar);

	if (BeginMenuBar()) {
		if (BeginMenu("Console")) {
			if (MenuItem("Clear")) { ClearLog(c); }
			ImGui::EndMenu();
		}
		EndMenuBar();
	}
	
	// Reserve enough left-over height for 1 separator + 1 input text
	//display scroll bar
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (BeginPopupContextWindow()){
		if (ImGui::Selectable("Clear")); //ClearLog();
		EndPopup();
	}

	//print previous text
	for (int i = 0; i < c->buffer.size(); i++) {
		//color formatting is "[c:red]text[c] text text"
		//TODO(o, sushi) store color information on the buffer
		//TODO(o, sushi) maybe optimize by only drawing what we know will be displayed on screen instead of parsing through all of it

		std::vector<std::pair<std::string, olc::Pixel>> text;

		std::smatch m;

		std::string s = c->buffer[i];

		int passes = 0;
		//TODO(o, sushi) figure out why this lags the engine when theres lots of text in the buffer
		//its probably cause its regex but regex works so well :/
		//see TODO above that could solve this issue
		//OR implement storing color on the buffer so we only do this when we add text to it
		//i'm not sure how we will store different colors per line though, maybe each element
		//will just need some kind of flag indicating if its to be put on a new line or not.
		while (std::regex_search(s, m, RegColorFormat)) { //parse text for color formatting

			//check if were dealing with a formatted part of the string
			if (std::regex_search(m[0].str(), std::regex("\\[c:[^\\]]+\\]"))) {
				//if we are, push the actual text with its color into text vector
				text.push_back(std::pair<std::string, olc::Pixel>(m[2].str(), colstrmap.at(m[1])));
			}
			else {
				//if we arent then just push the line into text vector
				text.push_back(std::pair<std::string, olc::Pixel>(m[0].str(), olc::BLANK));
			}
			passes++;
			s = m.suffix();
		}

		LOG(passes);

		for (std::pair<std::string, olc::Pixel> p : text) {
			if (p.second == olc::BLANK) {
				SameLine();
				TextWrapped(p.first.c_str());
			}
			else {
				PushStyleColor(ImGuiCol_Text, olcPixToVec4(p.second));
				SameLine();
				TextWrapped(p.first.c_str());
				PopStyleColor();
			}
		}
		TextWrapped("\n");
		//TextWrapped(c->buffer[i].c_str());
	}

	EndChild();


	//get input from text box
	bool reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
	PushStyleColor(ImGuiCol_FrameBg, olcPixToVec4(olc::DARK_CYAN));
	//PushStyleVar(ImguiStyle)
	if (InputText("/\\", c->inputBuf, sizeof(c->inputBuf), input_text_flags,&TextEditCallbackStub, (void*)this)){
		std::string s = c->inputBuf;
		reclaim_focus = true;

		c->history.push_back(s);

		AddLog(TOSTRING("[c:cyan]/\\[c] ", s), c); //print command typed
		//cut off arguments into their own string
		std::string args;
		size_t t = s.find_first_of(" ");
		if (t != std::string::npos) {
			args = s.substr(t);
			s.erase(t, s.size() - 1);
		}

		AddLog(ExecCommand(s, args, admin), c); //attempt to execute command and print result

		c->historyPos = -1; //reset history position

		memset(c->inputBuf, 0, sizeof(s)); //erase input from text box

	}
	PopStyleColor();


	SetItemDefaultFocus();
	if (reclaim_focus)
		SetKeyboardFocusHere(-1);


	ImGui::End();


	
}

void ConsoleSystem::Init() {
	locadmin = admin;
}

void ConsoleSystem::Update() {
	//DrawConsole();
	locadmin = admin;
	loccon = admin->console;
}