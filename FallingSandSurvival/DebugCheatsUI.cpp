#include "UIs.hpp"

#include "Textures.hpp"

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#include "ProfilerConfig.hpp"

bool DebugCheatsUI::visible = true;
std::vector<GPU_Image*> DebugCheatsUI::images = {};

void DebugCheatsUI::Setup() {
    EASY_FUNCTION(UI_PROFILER_COLOR);

    images = {};
    SDL_Surface* sfc = Textures::loadTexture("assets/objects/testPickaxe.png");
    images.push_back(GPU_CopyImageFromSurface(sfc));
    GPU_SetImageFilter(images[0], GPU_FILTER_NEAREST);
    SDL_FreeSurface(sfc);
    sfc = Textures::loadTexture("assets/objects/testHammer.png");
    images.push_back(GPU_CopyImageFromSurface(sfc));
    GPU_SetImageFilter(images[1], GPU_FILTER_NEAREST);
    SDL_FreeSurface(sfc);
    sfc = Textures::loadTexture("assets/objects/testVacuum.png");
    images.push_back(GPU_CopyImageFromSurface(sfc));
    GPU_SetImageFilter(images[2], GPU_FILTER_NEAREST);
    SDL_FreeSurface(sfc);
    sfc = Textures::loadTexture("assets/objects/testBucket.png");
    images.push_back(GPU_CopyImageFromSurface(sfc));
    GPU_SetImageFilter(images[3], GPU_FILTER_NEAREST);
    SDL_FreeSurface(sfc);
}

void DebugCheatsUI::Draw(Game* game) {
    EASY_FUNCTION(UI_PROFILER_COLOR);

    if(images.empty()) Setup();

	if(!visible) return;

    ImGui::SetNextWindowSize(ImVec2(40 * 5 + 16 - 4, 0));
    ImGui::SetNextWindowPos(ImVec2(15, 450), ImGuiCond_FirstUseEver);
	if(!ImGui::Begin("Cheats", NULL, 0)) {
		ImGui::End();
		return;
	}

    if(ImGui::CollapsingHeader("Give Item")) {
        ImGui::Indent();
        if(game->world == nullptr || game->world->player == nullptr){
            ImGui::Text("No player to give item");
        } else {
            int i = 0;
            ImGui::PushID(i);
            int frame_padding = 4;                            // -1 == uses default padding (style.FramePadding)
            ImVec2 size = ImVec2(48, 48);                     // Size of the image we want to make visible
            ImVec2 uv0 = ImVec2(0.0f, 0.0f);                  // UV coordinates for lower-left
            ImVec2 uv1 = ImVec2(1.0f, 1.0f);                  // UV coordinates for (32,32) in our texture
            ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);   // Black background
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // No tint

            ImTextureID texId = (ImTextureID)GPU_GetTextureHandle(images[i]);
            if(ImGui::ImageButton(texId, size, uv0, uv1, frame_padding, bg_col, tint_col)) {
                Item* i3 = new Item();
                i3->setFlag(ItemFlags::TOOL);
                i3->surface = Textures::loadTexture("assets/objects/testPickaxe.png");
                i3->texture = GPU_CopyImageFromSurface(i3->surface);
                GPU_SetImageFilter(i3->texture, GPU_FILTER_NEAREST);
                i3->pivotX = 2;
                game->world->player->setItemInHand(i3, game->world);
            }
            if(ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("%s", "Pickaxe");
                ImGui::EndTooltip();
            }
            ImGui::PopID();
            ImGui::SameLine();

            i++;

            ImGui::PushID(i);
            texId = (ImTextureID)GPU_GetTextureHandle(images[i]);
            if(ImGui::ImageButton(texId, size, uv0, uv1, frame_padding, bg_col, tint_col)) {
                Item* i3 = new Item();
                i3->setFlag(ItemFlags::HAMMER);
                i3->surface = Textures::loadTexture("assets/objects/testHammer.png");
                i3->texture = GPU_CopyImageFromSurface(i3->surface);
                GPU_SetImageFilter(i3->texture, GPU_FILTER_NEAREST);
                i3->pivotX = 2;
                game->world->player->setItemInHand(i3, game->world);
            }
            if(ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("%s", "Hammer");
                ImGui::EndTooltip();
            }
            ImGui::PopID();

            i++;

            ImGui::PushID(i);
            texId = (ImTextureID)GPU_GetTextureHandle(images[i]);
            if(ImGui::ImageButton(texId, size, uv0, uv1, frame_padding, bg_col, tint_col)) {
                Item* i3 = new Item();
                i3->setFlag(ItemFlags::VACUUM);
                i3->surface = Textures::loadTexture("assets/objects/testVacuum.png");
                i3->texture = GPU_CopyImageFromSurface(i3->surface);
                GPU_SetImageFilter(i3->texture, GPU_FILTER_NEAREST);
                i3->pivotX = 6;
                game->world->player->setItemInHand(i3, game->world);
            }
            if(ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("%s", "Vacuum");
                ImGui::EndTooltip();
            }
            ImGui::PopID();
            ImGui::SameLine();

            i++;

            ImGui::PushID(i);
            texId = (ImTextureID)GPU_GetTextureHandle(images[i]);
            if(ImGui::ImageButton(texId, size, uv0, uv1, frame_padding, bg_col, tint_col)) {
                Item* i3 = new Item();
                i3->setFlag(ItemFlags::FLUID_CONTAINER);
                i3->surface = Textures::loadTexture("assets/objects/testBucket.png");
                i3->capacity = 100;
                i3->loadFillTexture(Textures::loadTexture("assets/objects/testBucket_fill.png"));
                i3->texture = GPU_CopyImageFromSurface(i3->surface);
                GPU_SetImageFilter(i3->texture, GPU_FILTER_NEAREST);
                i3->pivotX = 0;
                game->world->player->setItemInHand(i3, game->world);
            }
            if(ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("%s", "Bucket");
                ImGui::EndTooltip();
            }
            ImGui::PopID();
            ImGui::SameLine();
        }
    }

	ImGui::End();
}
