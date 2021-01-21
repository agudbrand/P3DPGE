#include "RenderCanvasSystem.h"
#include "ConsoleSystem.h"
#include "../utils/GLOBALS.h"
#include "../ui/UI.h"
#include "../ui/UIContainer.h"
#include "../utils/ContainerManager.h"

#include "../components/Canvas.h"
#include "../components/Screen.h"
#include "../components/Scene.h"
#include "../components/Camera.h"
#include "../components/Input.h"

#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "../internal/imgui/imgui_impl_pge.h"
#include "../internal/imgui/imgui_impl_opengl2.h"

//// utility ui elements ////

void CopyButton(const char* text) {
	if(ImGui::Button("Copy")){ ImGui::LogToClipboard(); ImGui::LogText(text); ImGui::LogFinish(); }
}

void InputVector3(const char* id, Vector3* vecPtr, bool inputUpdate = false) {
	ImGui::SetNextItemWidth(-FLT_MIN);
	if(inputUpdate) { //pointer voodoo to treat Vector3 as float vector
		ImGui::InputFloat3(id, (float*)vecPtr); 
	} else {
		ImGui::InputFloat3(id, (float*)vecPtr, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue); 
	}
}

//// major ui elements ////

void MakeMenuBar(EntityAdmin* admin) {
	using namespace ImGui;
	static bool show_app_metrics = false;
	static bool show_app_about = false;
	static bool show_app_style_editor = false;
	if (show_app_metrics)       { ImGui::ShowMetricsWindow(&show_app_metrics); }
	if (show_app_about)         { ImGui::ShowAboutWindow(&show_app_about); }
	if (show_app_style_editor)	{ ImGui::Begin("Dear ImGui Style Editor", &show_app_style_editor); ImGui::ShowStyleEditor(); ImGui::End(); }

	if(BeginMenuBar()) {
		if(BeginMenu("Debug")) {
			static bool global_debug = true;
			if(MenuItem("global debug", 0, &global_debug)) { global_debug = !global_debug; admin->ExecCommand("debug_global"); }
			static bool print_commands = true;
			if(MenuItem("print commands", 0, &print_commands)) { print_commands = !print_commands; admin->ExecCommand("debug_command_exec"); }
			static bool pause_engine = false;
			if(MenuItem("pause engine", 0, &pause_engine)) { pause_engine = !pause_engine; admin->ExecCommand("time_pause_engine"); }
			if(MenuItem("next frame")) { admin->ExecCommand("time_next_frame"); }
			ImGui::EndMenu();
		}
		if(BeginMenu("Spawn")) {
			if(MenuItem("spawn box")) { admin->ExecCommand("spawn_box"); }
			if(BeginMenu("spawn complex")) {
				if(MenuItem("bmonkey")) { admin->ExecCommand("spawn_complex"); }
				if(MenuItem("whale_ship")) { admin->ExecCommand("spawn_complex1"); }
				if(MenuItem("24k_Triangles")) { admin->ExecCommand("spawn_complex2"); }
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (BeginMenu("Imgui")){
			MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
			MenuItem("Style Editor", NULL, &show_app_style_editor);
			ImGui::MenuItem("About Dear ImGui", NULL, &show_app_about);
			ImGui::EndMenu();
		}
		EndMenuBar();
	}
}

void MakeGeneralHeader(EntityAdmin* admin) {
	using namespace ImGui;
	Camera* camera = admin->currentCamera;
	if(CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen)) {
		if(TreeNodeEx("Camera", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
			Text("Address: %#08x", camera); SameLine(); if(ImGui::Button("Copy")) { ImGui::LogToClipboard(); ImGui::LogText("%#08x", camera); ImGui::LogFinish(); }
			Text("Position"); SameLine(); InputVector3("##camPos", &camera->position);
			Text("Rotation"); SameLine(); InputVector3("##camRot", &camera->rotation);
			if(BeginTable("split", 3)) {
				TableNextColumn(); Text("FOV"); SameLine(); InputFloat("##camFOV", &camera->fieldOfView, 0, 0, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
				TableNextColumn(); Text("NearZ"); SameLine(); InputFloat("##camNearZ", &camera->nearZ, 0, 0, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
				TableNextColumn(); Text("FarZ"); SameLine(); InputFloat("##camFarZ", &camera->farZ, 0, 0, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
				EndTable();
			}
			Separator();
		}
	}
}

void MakeEntitiesHeader(EntityAdmin* admin) {
	using namespace ImGui;
	if(CollapsingHeader("Entities")) {
		if(admin->singletonInput->selectedEntity) {
			Text("Selected Entity: %d", admin->singletonInput->selectedEntity->id);
		} else {
			Text("Selected Entity: None");
		}

		if(BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable)){
			TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
			TableSetupColumn("Components");
			TableHeadersRow();
			int counter = 0;
			for(auto& entity : admin->entities) {
				counter++;
				TableNextRow(); TableNextColumn();
				if(ImGui::Button(std::to_string(entity.first).c_str())) {
					admin->singletonInput->selectedEntity = entity.second;
				}
				
				TableNextColumn();
				Text("Address: %#08x", entity.second);
				if(TreeNodeEx((std::string("comps") + std::to_string(entity.first)).c_str(), ImGuiTreeNodeFlags_NoTreePushOnOpen, "Components")) {
					for(Component* comp : entity.second->components) {
						//TODO(delle) implement components list on entities
					}
					Separator();
				}
			}
			EndTable();
		}
	}
}

void MakeRenderHeader(EntityAdmin* admin) {
	using namespace ImGui;
	Scene* scene = admin->currentScene;
	if (CollapsingHeader("Render options")){
		if (BeginTable("split", 3)) {
			TableNextColumn(); Checkbox("wireframe", &scene->RENDER_WIREFRAME);
			TableNextColumn(); Checkbox("textures", &scene->RENDER_TEXTURES);
			TableNextColumn(); Checkbox("edge numbers", &scene->RENDER_EDGE_NUMBERS);
			TableNextColumn(); Checkbox("local axis", &scene->RENDER_LOCAL_AXIS);
			TableNextColumn(); Checkbox("global axis", &scene->RENDER_GLOBAL_AXIS);
			TableNextColumn(); Checkbox("transforms", &scene->RENDER_TRANSFORMS);
			TableNextColumn(); Checkbox("physics vectors", &scene->RENDER_PHYSICS);
			TableNextColumn(); Checkbox("screen aabb", &scene->RENDER_SCREEN_BOUNDING_BOX);
			TableNextColumn(); Checkbox("mesh vertices", &scene->RENDER_MESH_VERTICES);
			TableNextColumn(); Checkbox("mesh normals", &scene->RENDER_MESH_NORMALS);
			TableNextColumn(); Checkbox("grid", &scene->RENDER_GRID);
			TableNextColumn(); Checkbox("light rays", &scene->RENDER_LIGHT_RAYS);
			EndTable();
		}
	}
}

void MakeBufferlogHeader(EntityAdmin* admin) {
	using namespace ImGui;
	if(CollapsingHeader("Bufferlog")) {
		for(auto str : g_cBuffer.container) {
			if(str.has_value()) {
				Text(str.get().c_str());
			}
		}
	}
}

//this actually creates the debug tools panel
void MakeP3DPGEDebugTools(EntityAdmin* admin) {
	using namespace ImGui;
	ImGui::Begin("P3DPGE Debug Tools", 0, ImGuiWindowFlags_MenuBar);

	MakeMenuBar(admin);
	MakeGeneralHeader(admin);
	MakeEntitiesHeader(admin);
	MakeRenderHeader(admin);
	MakeBufferlogHeader(admin);
	
	ImGui::End();
}

void RenderCanvasSystem::DrawUI(void) {
	using namespace ImGui;
	//These 3 lines are mandatory per-frame initialization
	ImGui_ImplOpenGL2_NewFrame();
	admin->tempCanvas->pge_imgui->ImGui_ImplPGE_NewFrame();
	ImGui::NewFrame();

	//demo window for reference
	//ImGui::ShowDemoWindow();

	////////////////////////////////////////////

	ImGuiIO& io = ImGui::GetIO();
	static bool showDebugTools = true;
	if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)) && io.KeyCtrl && io.KeyShift) { showDebugTools = !showDebugTools; }
	if(showDebugTools) MakeP3DPGEDebugTools(admin);
	//TODO(i, sushi) add console boolean
	//cant use a static call here because then we can't reference admin in the static function
	//maybe theres a better way to do this, I don't know
	for (System* s : admin->systems) {
		if (ConsoleSystem* c = dynamic_cast<ConsoleSystem*>(s)) {
			c->DrawConsole();
		}
	}

	////////////////////////////////////////////

	//This finishes the Dear ImGui and renders it to the screen
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void RenderCanvasSystem::Init() {
	Canvas* canvas = admin->tempCanvas;
	olc::PixelGameEngine* p =	admin->p;

	//One time initialization of the Dear ImGui library
	ImGui::CreateContext();
	//Create an instance of the Dear ImGui PGE Integration
	canvas->pge_imgui = new olc::imgui::PGE_ImGUI();
		
	//The vi2d for pixel size must match the values given in Construct()
	//Otherwise the mouse will not work correctly
	canvas->pge_imgui->ImGui_ImplPGE_Init(p);
	//Initialize the OpenGL2 rendering system
	ImGui_ImplOpenGL2_Init();

	//Set a custom render function on layer 0.  Since DrawUI is a member of
	//our class, we need to use std::bind
	p->SetLayerCustomRenderFunction(0, std::bind(&RenderCanvasSystem::DrawUI, this));
}

void RenderCanvasSystem::Update() {
	Canvas*	canvas =			admin->tempCanvas;
	
	olc::PixelGameEngine* p =	admin->p;

	if(!canvas->hideAll) {
		for(UIContainer* con : canvas->containers) {
			for(UI* ui : con->container) {
				ui->Draw(p);
			}
		}
	}

	//p->DrawStringDecal(olc::vf2d(screen->width - 300, screen->height - 20), "Mouse: " + screen->mousePosV3.str2f());
}