#include "RenderCanvasSystem.h"
#include "../utils/GLOBALS.h"
#include "../ui/UI.h"
#include "../ui/UIContainer.h"

#include "../components/Canvas.h"
#include "../components/Screen.h"
#include "../components/Scene.h"

#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "../internal/imgui/imgui_impl_pge.h"
#include "../internal/imgui/imgui_impl_opengl2.h"

void RenderCanvasSystem::DrawUI(void) {
	Scene* scene = admin->currentScene;

	using namespace ImGui;
	//These 3 lines are mandatory per-frame initialization
	ImGui_ImplOpenGL2_NewFrame();
	admin->tempCanvas->pge_imgui->ImGui_ImplPGE_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	////////////////////////////////////////////

	static bool show_app_metrics = false;
	static bool show_app_style_editor = false;
	if (show_app_metrics)       { ImGui::ShowMetricsWindow(&show_app_metrics); }
	if (show_app_style_editor)	{ ImGui::Begin("Dear ImGui Style Editor", &show_app_style_editor); ImGui::ShowStyleEditor(); ImGui::End(); }

	ImGui::Begin("P3DPGE Debug Menu", 0, ImGuiWindowFlags_MenuBar);
	if(BeginMenuBar()) {
		if(BeginMenu("Debug")) {
			static bool global_debug = true;
			if(MenuItem("global debug", 0, &global_debug)) { global_debug = !global_debug; admin->ExecCommand("debug_global"); }
			static bool print_commands = true;
			if(MenuItem("print commands", 0, &print_commands)) { print_commands = !print_commands; admin->ExecCommand("debug_command_exec"); }
			static bool pause_engine = false;
			if(MenuItem("pause engine", 0, &pause_engine)) { pause_engine = !pause_engine; admin->ExecCommand("time_pause_engine"); }
			if(MenuItem("next frame")) { admin->ExecCommand("time_next_frame"); }
			if(MenuItem("reset camera")) { admin->ExecCommand("reset_camera"); }
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
		if (ImGui::BeginMenu("Tools")){
            ImGui::MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
            ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
            ImGui::EndMenu();
        }
		EndMenuBar();
	}

	if (ImGui::CollapsingHeader("Entities")){
        //TODO(delle) implement Entities list and selection thru it, see Child windows
    }

	if (ImGui::CollapsingHeader("Selected entity")){
        //TODO(delle) implement selected entity panel, see PropertyEditor
    }

	if (ImGui::CollapsingHeader("Render options")){
        if (ImGui::BeginTable("split", 3)) {
            ImGui::TableNextColumn(); ImGui::Checkbox("wireframe", &scene->RENDER_WIREFRAME);
            ImGui::TableNextColumn(); ImGui::Checkbox("textures", &scene->RENDER_TEXTURES);
            ImGui::TableNextColumn(); ImGui::Checkbox("edge numbers", &scene->RENDER_EDGE_NUMBERS);
            ImGui::TableNextColumn(); ImGui::Checkbox("local axis", &scene->RENDER_LOCAL_AXIS);
            ImGui::TableNextColumn(); ImGui::Checkbox("global axis", &scene->RENDER_GLOBAL_AXIS);
            ImGui::TableNextColumn(); ImGui::Checkbox("transforms", &scene->RENDER_TRANSFORMS);
            ImGui::TableNextColumn(); ImGui::Checkbox("physics vectors", &scene->RENDER_PHYSICS);
            ImGui::TableNextColumn(); ImGui::Checkbox("screen aabb", &scene->RENDER_SCREEN_BOUNDING_BOX);
            ImGui::TableNextColumn(); ImGui::Checkbox("mesh vertices", &scene->RENDER_MESH_VERTICES);
            ImGui::TableNextColumn(); ImGui::Checkbox("mesh normals", &scene->RENDER_MESH_NORMALS);
            ImGui::TableNextColumn(); ImGui::Checkbox("grid", &scene->RENDER_GRID);
            ImGui::TableNextColumn(); ImGui::Checkbox("light rays", &scene->RENDER_LIGHT_RAYS);
            ImGui::EndTable();
        }
    }

	if (ImGui::CollapsingHeader("Bufferlog")){
        //TODO(delle) implement bufferlog, see Child windows
    }
	ImGui::End();

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
	Screen*	screen =			admin->singletonScreen;
	olc::PixelGameEngine* p =	admin->p;

	if(!canvas->hideAll) {
		for(UIContainer* con : canvas->containers) {
			for(UI* ui : con->container) {
				ui->Draw(p);
			}
		}
	}

	p->DrawStringDecal(olc::vf2d(screen->width - 300, screen->height - 20), "Mouse: " + screen->mousePosV3.str2f());
}