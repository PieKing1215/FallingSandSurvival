#pragma once

class Settings {
public:
    static bool draw_frame_graph;
    static bool draw_background;
    static bool draw_background_grid;
    static bool draw_load_zones;
    static bool draw_physics_meshes;
    static bool draw_chunk_state;
    static bool draw_chunk_queue;
    static bool draw_material_info;
    static bool draw_uinode_bounds;
    static bool draw_light_map;
    static bool draw_temperature_map;
    static bool draw_shaders;
    static bool tick_world;
    static bool tick_box2d;
    static bool tick_temperature;
    static bool hd_objects;

    static int hd_objects_size;
};
