#pragma once

class Settings {
public:

    static bool draw_frame_graph;
    static bool draw_background;
    static bool draw_background_grid;
    static bool draw_load_zones;
    static bool draw_physics_debug;
    static bool draw_b2d_shape;
    static bool draw_b2d_joint;
    static bool draw_b2d_aabb;
    static bool draw_b2d_pair;
    static bool draw_b2d_centerMass;
    static bool draw_chunk_state;
    static bool draw_debug_stats;
    static bool draw_material_info;
    static bool draw_detailed_material_info;
    static bool draw_uinode_bounds;
    static bool draw_temperature_map;

    static bool draw_shaders;
    static int water_overlay;
    static bool water_showFlow;
    static bool water_pixelated;
    static float lightingQuality;
    static bool draw_light_overlay;
    static bool simpleLighting;
    static bool lightingEmission;
    static bool lightingDithering;

    static bool tick_world;
    static bool tick_box2d;
    static bool tick_temperature;
    static bool hd_objects;

    static int hd_objects_size;
};
