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

    ImGui::SetNextWindowSize(ImVec2(200, 0));
    ImGui::SetNextWindowPos(ImVec2(game->WIDTH - 200 - 15, 25), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Debug Settings", NULL, ImGuiWindowFlags_NoResize)) {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Draw Frame Graph"    , &Settings::draw_frame_graph);

    ImGui::Checkbox("Draw Debug Stats", &Settings::draw_debug_stats);
    ImGui::Checkbox("Draw Chunk States", &Settings::draw_chunk_state);
    ImGui::Checkbox("Draw Load Zones"     , &Settings::draw_load_zones);
    ImGui::Checkbox("Draw Physics Meshes" , &Settings::draw_physics_meshes);
    ImGui::Checkbox("Draw Material Info"  , &Settings::draw_material_info);
    ImGui::Checkbox("Draw Temperature Map", &Settings::draw_temperature_map);
    ImGui::Checkbox("Draw Shaders"        , &Settings::draw_shaders);

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

    if(Settings::draw_shaders) {
        ImGui::SetNextItemWidth(80);
        ImGui::Indent(10.0f);
        ImGui::SliderFloat("Light Quality", &Settings::lightingQuality, 0.0, 1.0, "", 0);
        ImGui::Checkbox("Light Overlay", &Settings::draw_light_overlay);
        ImGui::Checkbox("Simple Lighting", &Settings::simpleLighting);
        ImGui::Checkbox("Lighting Emission", &Settings::lightingEmission);
        ImGui::Unindent(10.0f);
    }

    ImGui::Checkbox("Tick World"          , &Settings::tick_world);
    ImGui::Checkbox("Tick Box2D"          , &Settings::tick_box2d);
    ImGui::Checkbox("Tick Temperature"    , &Settings::tick_temperature);

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

    ImGui::End();
}
