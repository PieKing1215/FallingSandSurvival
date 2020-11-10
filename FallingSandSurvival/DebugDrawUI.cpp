#pragma once

#include "UIs.hpp"

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#include "ProfilerConfig.hpp"

bool DebugDrawUI::visible = true;
int DebugDrawUI::selIndex = -1;
std::vector<GPU_Image*> DebugDrawUI::images = {};
uint8 DebugDrawUI::brushSize = 5;
Material* DebugDrawUI::selectedMaterial = &Materials::GENERIC_AIR;

void DebugDrawUI::Setup() {
    EASY_FUNCTION(UI_PROFILER_COLOR);

    images = {};
    for(size_t i = 0; i < Materials::MATERIALS.size(); i++) {
        Material* mat = Materials::MATERIALS[i];
        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 16, 16, 32, SDL_PIXELFORMAT_ARGB8888);
        for(int x = 0; x < surface->w; x++) {
            for(int y = 0; y < surface->h; y++) {
                MaterialInstance m = Tiles::create(mat, x, y);
                PIXEL(surface, x, y) = m.color + (m.mat->alpha << 24);
            }
        }
        images.push_back(GPU_CopyImageFromSurface(surface));
        GPU_SetImageFilter(images[i], GPU_FILTER_NEAREST);
        SDL_FreeSurface(surface);
    }
}

void DebugDrawUI::Draw(Game* game) {
    EASY_FUNCTION(UI_PROFILER_COLOR);

    if(images.empty()) Setup();

	if(!visible) return;

    int width = 5;

    int nRows = ceil(Materials::MATERIALS.size() / (float)width);

    ImGui::SetNextWindowSize(ImVec2(40 * width + 16 + 20, 70 + 5 * 40));
    ImGui::SetNextWindowPos(ImVec2(15, 25), ImGuiCond_FirstUseEver);
	if(!ImGui::Begin("Debug Draw", NULL, ImGuiWindowFlags_NoResize)) {
		ImGui::End();
		return;
	}

    auto a = selIndex == -1 ? "None" : selectedMaterial->name;
    ImGui::Text("Selected: %s", a.c_str());
    ImGui::Text("Brush Size: %d", brushSize);

    ImGui::Separator();

    ImGui::BeginChild("MaterialList", ImVec2(0, 0), false);
    ImGui::Indent(5);
    for(size_t i = 0; i < Materials::MATERIALS.size(); i++) {
        int x = (int)(i % width);
        int y = (int)(i / width);

        if(x > 0)
            ImGui::SameLine();
        ImGui::PushID((int)i);

        ImVec2 selPos = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos(ImVec2(selPos.x, selPos.y + (x != 0 ? -1 : 0)));
        if(ImGui::Selectable("", selIndex == i, 0, ImVec2(32, 36))) {
            selIndex = (int)i;
            selectedMaterial = Materials::MATERIALS[i];
        }

        if(ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", Materials::MATERIALS[i]->name.c_str());
            ImGui::EndTooltip();
        }

        ImVec2 prevPos = ImGui::GetCursorScreenPos();
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::SetCursorScreenPos(ImVec2(selPos.x - 1, selPos.y + (x == 0 ? 1 : 0)));

        // imgui_impl_opengl3.cpp implements ImTextureID as GLuint 
        ImTextureID texId = (ImTextureID)GPU_GetTextureHandle(images[i]);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
        ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
        ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);

        ImGui::Image(texId, ImVec2(32, 32), uv_min, uv_max, tint_col, border_col);

        ImGui::SetCursorScreenPos(prevPos);


        ImGui::PopID();

        /*mn->hoverCallback = [hoverMaterialLabel](Material* mat) {
            hoverMaterialLabel->text = mat->name;
            hoverMaterialLabel->updateTexture();
        };
        mn->selectCallback = [&](Material* mat) {
            selectMaterialLabel->text = mat->name;
            selectMaterialLabel->updateTexture();
            selectedMaterial = mat;
        };*/
    }

    ImGui::Unindent(5);
    ImGui::EndChild();

	ImGui::End();
}
