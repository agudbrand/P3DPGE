#include "ConsoleSystem.h"

#include "../components/Input.h"
#include "../components/Screen.h"
#include "../components/Console.h"

#include "../utils/Command.h"

#include "../internal/imgui/imgui_impl_pge.h"
#include "../internal/imgui/imgui_impl_opengl2.h"

#define RegColorFormat std::regex("(?:\\[c:([^\\]]*)\\]([^\\]]*)\\[c\\]|([^\\[]+))")

using namespace ImGui;

EntityAdmin* locadmin; //so I can access admin in TextEditCallback
Console* loccon; //so I can access console in TextEditCallback

bool sel_com = false; //true when selecting an auto complete possibility
bool sel_com_ret = false; //tells the callback function that it is going to replace text
std::string sel_com_str = ""; //the string we're replacing input with
std::vector<std::string> posis;
//int match_sel = 0;

std::map<std::string, olc::Pixel> colstrmap{ //TODO(, sushi) extend this map
	{"red", olc::RED},
	{"dred", olc::DARK_RED},
	{"blue", olc::BLUE},
	{"dblue", olc::DARK_BLUE},
	{"cyan", olc::CYAN},
	{"dcyan", olc::DARK_CYAN},
	{"grey", olc::GREY},
	{"dgrey", olc::DARK_GREY},
	{"green", olc::GREEN},
	{"dgreen", olc::DARK_GREEN},
	{"yellow", olc::YELLOW},
	{"dyellow", olc::DARK_YELLOW},
	{"magen", olc::MAGENTA},
	{"dmagen", olc::DARK_MAGENTA},
	{"black", olc::BLACK}
};

ImVec4 olcPixToVec4(olc::Pixel p) {
	return ImVec4((float)p.r / 255, (float)p.g / 255, (float)p.b / 255, p.a / 255);
}

void AddLog(std::string input, Console* c) {
	
	std::smatch m;

	while (std::regex_search(input, m, RegColorFormat)) { //parse text for color formatting

		//check if were dealing with a formatted part of the string
		if (std::regex_search(m[0].str(), std::regex("\\[c:[^\\]]+\\]"))) {
			//if we are, push the actual text with its color into text vector
			c->buffer.push_back(std::pair<std::string, olc::Pixel>(m[2].str(), colstrmap.at(m[1])));
		}
		else {
			//if we arent then just push the line into text vector
			c->buffer.push_back(std::pair<std::string, olc::Pixel>(m[0].str(), olc::BLANK));
		}
		input = m.suffix();
	}
	c->buffer[c->buffer.size() - 1].first += "\n";
}

void ClearLog(Console* c) {
	c->buffer.clear();
}

std::string ExecCommand(std::string command, std::string args, EntityAdmin* admin) {
	if (admin->commands.find(command) != admin->commands.end()) {
		return admin->commands.at(command)->Exec(admin, args);
	}
	else {
		return "[c:red]Command[c] \"" + command + "\" [c:red]not found.[c]";
	}

	//admin->
}

int TextEditCallback(ImGuiInputTextCallbackData* data) {
	switch (data->EventFlag) {
	case ImGuiInputTextFlags_CallbackCompletion:{
			std::string input = data->Buf;

			int fwordl = 0;

			if (std::regex_search(input, std::regex("^.+ +"))) {
				fwordl = input.find_first_of(" ") + 1;
				input.erase(0, input.find_first_of(" ") + 1);

			}

			std::regex e("^" + input + ".*");
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
				data->DeleteChars(fwordl, data->BufTextLen - fwordl);
				data->InsertChars(data->CursorPos, posi[0].c_str());
			}
			else { //if there are multiple we handle them in a selection table
				posis.clear();
				posis = posi;
				sel_com = true;

				
			}

			loccon->scrollToBottom = true; //scroll to bottom when auto completing 

			break;
		}
	case ImGuiInputTextFlags_CallbackHistory: {

		const int prev_hist_pos = loccon->historyPos;
		if (data->EventKey == ImGuiKey_UpArrow) {
			if (loccon->historyPos == -1) {
				loccon->historyPos = loccon->history.size() - 1;
			}
			else if (loccon->historyPos > 0) {
				loccon->historyPos--;
			}
		}
		else if (data->EventKey == ImGuiKey_DownArrow) {
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
	case ImGuiInputTextFlags_CallbackAlways: {
		if (sel_com_ret) {
			std::string str = data->Buf;

			int fwordl = 0; //we need to make sure we don't override valid input
			if (std::regex_search(str, std::regex("^.+ +"))) {
				fwordl = str.find_first_of(" ") + 1;
				str.erase(0, str.find_first_of(" ") + 1);
			}

			str += sel_com_str;

			data->DeleteChars(0, data->BufTextLen - fwordl);
			data->InsertChars(data->BufTextLen, str.c_str());
			sel_com_ret = false;
		}
	}
	}

	return 0;
}



static int TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
	return TextEditCallback(data);
}

void DoNothing() {

}

void ConsoleSystem::DrawConsole() {

	Console* c = admin->console;

	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO();

	style.AntiAliasedFill = false;
	style.AntiAliasedLines = false;
	style.AntiAliasedLinesUseTex = false;

	//for some reason these werent set in the actual backend and it was causing issues
	io.BackendFlags = ImGuiBackendFlags_HasGamepad | ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.ConfigWindowsResizeFromEdges = true;

	//window styling
	PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 0);
	PushStyleColor(ImGuiCol_Border,               ImVec4(0, 0, 0, 0)); //this doesn't work with any color for some reason it always makes it as bright as possible
	PushStyleColor(ImGuiCol_TitleBg,              ImVec4(0, 0, 0, 255));
	PushStyleColor(ImGuiCol_WindowBg,             ImVec4(0, 0, 0, 255));
	PushStyleColor(ImGuiCol_TitleBgActive,        ImVec4(0, 0, 0, 255));
	PushStyleColor(ImGuiCol_ScrollbarGrab,        ImVec4(37, 36, 36, 255));
	PushStyleColor(ImGuiCol_ScrollbarGrabActive,  ImVec4(0, 94, 83, 255));
	PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ImVec4(48, 85, 90, 255));
	PushStyleColor(ImGuiCol_ResizeGrip,           ImVec4(0, 0, 0, 0)); //ugly thing but its still there
	PushStyleColor(ImGuiCol_ResizeGripActive,     ImVec4(66, 150, 250, 242));
	PushStyleColor(ImGuiCol_ResizeGripHovered,    ImVec4(115, 166, 227, 171));

	//initialize console window
	SetNextWindowSize(ImVec2(admin->screen->width, admin->screen->height / 1.5));
	SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin(TOSTRING("size: ", c->buffer.size()).c_str(), 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);


	if (BeginMenuBar()) {
		if (BeginMenu("Console")) {
			if (MenuItem("Clear")) { ClearLog(c); }
			if (MenuItem("Autoscroll", 0, &c->autoScroll)) { c->autoScroll = !c->autoScroll; }
			ImGui::EndMenu();
		}
		if (BeginMenu("Debug")) {
			if (MenuItem("Throw CASSERT")) { CASSERT(false, "cool"); }
			if (MenuItem("Throw CASSERTWARN")) { CASSERTWARN(false, "yep"); }
			ImGui::EndMenu();
		}
		EndMenuBar();
	}
	static bool reclaim_focus = false;

	//display completion table
	//this could probably be done in a better way but idc it works
	static int match_sel = 0;
	bool ok_flag = false;
	if (sel_com) {
		bool selected = false;
		bool escape = false;
		if (admin->input->KeyPressed(olc::DOWN) && match_sel < posis.size() - 1) { match_sel++; }
		if (admin->input->KeyPressed(olc::UP)   && match_sel > 0)                { match_sel--; }
		if (admin->input->KeyPressed(olc::ENTER)) { selected = true; }
		if (admin->input->KeyPressed(olc::ESCAPE)) { escape = true; match_sel = 0; reclaim_focus = true; }

		if (escape) { ok_flag = true; }
		else {
			ImGui::SetNextItemOpen(1);
			if (TreeNode("match table")) {
				if (BeginChild("matchScroll", ImVec2(0, 100), false)) {
					if (BeginTable("match table", 1, ImGuiTableFlags_BordersH)) {//posi.size())) {

						int i = 0;
						for (std::string s : posis) {
							TableNextColumn();
							if (i == match_sel) {
								SetScrollHereY(0);
								PushStyleColor(ImGuiCol_Text, olcPixToVec4(olc::RED));
								Text(s.c_str());
								PopStyleColor();
								if (selected) {
									sel_com_ret = true;
									sel_com_str = s;
									ok_flag = true;
									match_sel = 0;
									break;
								}
							}
							else {
								Text(s.c_str());
							}
							i++;
						}
						EndTable();
					}
					EndChild();
				}
				TreePop();
			}
		}
		
	}
	
	// Reserve enough left-over height for 1 separator + 1 input text
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (BeginPopupContextWindow()){
		if (ImGui::Selectable("Clear")); //ClearLog();
		EndPopup();
	}

	//print previous text
	for (std::pair<std::string, olc::Pixel> p : c->buffer) {
		//color formatting is "[c:red]text[c] text text"
		//TODO(o, sushi) maybe optimize by only drawing what we know will be displayed on screen instead of parsing through all of it

		if (p.second == olc::BLANK) {
			SameLine(0, 0);
			TextWrapped(p.first.c_str());
		}
		else {
			PushStyleColor(ImGuiCol_Text, olcPixToVec4(p.second));
			SameLine(0, 0);
			TextWrapped(p.first.c_str());
			PopStyleColor();
		}
		
		if (p.first[p.first.size() - 1] == '\n') {
			TextWrapped("\n");
		}
		//TextWrapped(c->buffer[i].c_str());
	}

	//auto scroll window
	if (c->scrollToBottom || (c->autoScroll && GetScrollY() >= GetScrollMaxY())) SetScrollHereY(1);
	c->scrollToBottom = false;
	
	EndChild();
	
	//get input from text box
	ImGuiInputTextFlags input_text_flags = 0;
	if (!sel_com)  input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackAlways;
	else  input_text_flags = 0;
	

	PushStyleColor(ImGuiCol_FrameBg, olcPixToVec4(olc::VERY_DARK_CYAN));
	SetNextItemWidth(ImGui::GetWindowWidth() - 50);
	if (InputText("/\\", c->inputBuf, sizeof(c->inputBuf), input_text_flags, &TextEditCallbackStub, (void*)this)) { 

		std::string s = c->inputBuf;
		reclaim_focus = true;

		c->history.push_back(s);

		AddLog(TOSTRING("[c:cyan]/[c][c:dcyan]\\[c] ", s), c); //print command typed

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

		c->scrollToBottom = true; //scroll to bottom when we press enter
	}

	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1);

	//reclaim_focus ? ImGui::SetKeyboardFocusHere(-1) : DoNothing();

	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1); //for some reason this only works when there is no if statement

	reclaim_focus = false;

	admin->IMGUI_KEY_CAPTURE = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		
		
	         PopStyleColor();             PopStyleColor(); PopStyleColor();              PopStyleVar();
	            PopStyleColor();       PopStyleColor();       PopStyleColor();        PopStyleColor();
			       PopStyleColor(); PopStyleColor();             PopStyleColor();  PopStyleColor(); 
	                     
	                   

	//if we selected something from completion menu
	//we have to do this here to prevent enter from sending a command
	if (ok_flag) { sel_com = false; }
	
	
	ImGui::End();
}

//this must be a separate funciton because TextEditCallback had a fit when I tried
//making this the main AddLog function
void ConsoleSystem::PushConsole(std::string s) {
	AddLog(s, admin->console);
}

void ConsoleSystem::Init() {
	locadmin = admin;
	loccon = admin->console;
	AddLog("[c:dcyan]P3DPGE Console[c] [c:cyan]/[c][c:dcyan]\\[c]", loccon);
	AddLog("\"listc\" for a list of commands\n\"help {command}\" to view a commands help page", loccon);
	AddLog("\n[c:dyellow]Console TODOS:[c]", loccon);
	AddLog(
		"> implement argument completion for commands\n"
		"> implement arguments for commands that need them\n"
		"> add help to commands that don't have a descriptive help yet\n"
		"> figure out why the text box doesn't set focus after pressing enter\n", loccon);
}

void ConsoleSystem::Update() {
	//DrawConsole();
	locadmin = admin;
	loccon = admin->console;
}