#pragma once

#include "UIs.hpp"

#include "Settings.hpp"

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#include "ProfilerConfig.hpp"

bool DebugUI::visible = true;

void DebugUI::Draw(Game* game) {
    EASY_FUNCTION(UI_PROFILER_COLOR);

    if(!visible) return;

    ImGui::SetNextWindowSize(ImVec2(250, 0));
    ImGui::SetNextWindowPos(ImVec2(game->WIDTH - 200 - 15, 25), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Debug Settings", NULL, ImGuiWindowFlags_NoResize)) {
        ImGui::End();
        return;
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::TreeNode("Rendering")) {

        ImGui::Checkbox("Draw Frame Graph", &Settings::draw_frame_graph);
        ImGui::Checkbox("Draw Debug Stats", &Settings::draw_debug_stats);

        ImGui::Checkbox("Draw Chunk States", &Settings::draw_chunk_state);
        ImGui::Checkbox("Draw Load Zones", &Settings::draw_load_zones);

        ImGui::Checkbox("Material Tooltips", &Settings::draw_material_info);
        ImGui::Indent(10.0f);
        ImGui::Checkbox("Debug Material Tooltips", &Settings::draw_detailed_material_info);
        ImGui::Unindent(10.0f);

        if(ImGui::TreeNode("Physics")) {
            ImGui::Checkbox("Draw Physics Debug", &Settings::draw_physics_debug);

            if(ImGui::TreeNode("Box2D")) {
                ImGui::Checkbox("shape", &Settings::draw_b2d_shape);
                ImGui::Checkbox("joint", &Settings::draw_b2d_joint);
                ImGui::Checkbox("aabb", &Settings::draw_b2d_aabb);
                ImGui::Checkbox("pair", &Settings::draw_b2d_pair);
                ImGui::Checkbox("center of mass", &Settings::draw_b2d_centerMass);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Shaders")) {
            if(ImGui::Button("Reload Shaders")) {
                game->loadShaders();
            }
            ImGui::Checkbox("Draw Shaders", &Settings::draw_shaders);

            if(ImGui::TreeNode("Lighting")) {
                ImGui::SetNextItemWidth(80);
                ImGui::SliderFloat("Quality", &Settings::lightingQuality, 0.0, 1.0, "", 0);
                ImGui::Checkbox("Overlay", &Settings::draw_light_overlay);
                ImGui::Checkbox("Simple", &Settings::simpleLighting);
                ImGui::Checkbox("Emission", &Settings::lightingEmission);
                ImGui::Checkbox("Ditheirng", &Settings::lightingDithering);

                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Water")) {
                const char* items[] = {"off", "flow map", "distortion"};
                const char* combo_label = items[Settings::water_overlay];
                ImGui::SetNextItemWidth(80 + 24);
                if(ImGui::BeginCombo("Overlay", combo_label, 0)) {
                    for(int n = 0; n < IM_ARRAYSIZE(items); n++) {
                        const bool is_selected = (Settings::water_overlay == n);
                        if(ImGui::Selectable(items[n], is_selected)) {
                            Settings::water_overlay = n;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if(is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::Checkbox("Show Flow", &Settings::water_showFlow);
                ImGui::Checkbox("Pixelated", &Settings::water_pixelated);

                ImGui::TreePop();
            }


            ImGui::TreePop();
        }

        if(ImGui::TreeNode("World")) {

            ImGui::Checkbox("Draw Temperature Map", &Settings::draw_temperature_map);

            if(ImGui::Checkbox("Draw Background", &Settings::draw_background)) {
                for(int x = 0; x < game->world->width; x++) {
                    for(int y = 0; y < game->world->height; y++) {
                        game->world->dirty[x + y * game->world->width] = true;
                        game->world->layer2Dirty[x + y * game->world->width] = true;
                    }
                }
            }

            if(ImGui::Checkbox("Draw Background Grid", &Settings::draw_background_grid)) {
                for(int x = 0; x < game->world->width; x++) {
                    for(int y = 0; y < game->world->height; y++) {
                        game->world->dirty[x + y * game->world->width] = true;
                        game->world->layer2Dirty[x + y * game->world->width] = true;
                    }
                }
            }

            if(ImGui::Checkbox("HD Objects", &Settings::hd_objects)) {
                GPU_FreeTarget(game->textureObjects->target);
                GPU_FreeImage(game->textureObjects);
                GPU_FreeTarget(game->textureObjectsBack->target);
                GPU_FreeImage(game->textureObjectsBack);
                GPU_FreeTarget(game->textureEntities->target);
                GPU_FreeImage(game->textureEntities);

                game->textureObjects = GPU_CreateImage(
                    game->world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), game->world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),
                    GPU_FormatEnum::GPU_FORMAT_RGBA
                );
                GPU_SetImageFilter(game->textureObjects, GPU_FILTER_NEAREST);

                game->textureObjectsBack = GPU_CreateImage(
                    game->world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), game->world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),
                    GPU_FormatEnum::GPU_FORMAT_RGBA
                );
                GPU_SetImageFilter(game->textureObjectsBack, GPU_FILTER_NEAREST);

                GPU_LoadTarget(game->textureObjects);
                GPU_LoadTarget(game->textureObjectsBack);

                game->textureEntities = GPU_CreateImage(
                    game->world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), game->world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),
                    GPU_FormatEnum::GPU_FORMAT_RGBA
                );
                GPU_SetImageFilter(game->textureEntities, GPU_FILTER_NEAREST);

                GPU_LoadTarget(game->textureEntities);
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if(ImGui::TreeNode("Simulation")) {
        ImGui::Checkbox("Tick World", &Settings::tick_world);
        ImGui::Checkbox("Tick Box2D", &Settings::tick_box2d);
        ImGui::Checkbox("Tick Temperature", &Settings::tick_temperature);

        ImGui::TreePop();
    }
    

    ImGui::End();
}
