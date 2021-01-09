
#include "Settings.hpp"

bool Settings::draw_frame_graph     = true;
bool Settings::draw_background      = true;
bool Settings::draw_background_grid = false;
bool Settings::draw_load_zones      = false;
bool Settings::draw_physics_debug   = false;
bool Settings::draw_b2d_shape       = true;
bool Settings::draw_b2d_joint       = false;
bool Settings::draw_b2d_aabb        = false;
bool Settings::draw_b2d_pair        = false;
bool Settings::draw_b2d_centerMass  = true;
bool Settings::draw_chunk_state     = false;
bool Settings::draw_debug_stats     = false;
bool Settings::draw_material_info   = true;
bool Settings::draw_detailed_material_info = true;
bool Settings::draw_temperature_map = false;

bool Settings::draw_shaders		    = true;
int Settings::water_overlay         = 0;
bool Settings::water_showFlow       = true;
bool Settings::water_pixelated      = false;
float Settings::lightingQuality     = 0.5f;
bool Settings::draw_light_overlay   = false;
bool Settings::simpleLighting       = false;
bool Settings::lightingEmission     = true;
bool Settings::lightingDithering    = false;

bool Settings::tick_world           = true;
bool Settings::tick_box2d           = true;
bool Settings::tick_temperature     = true;
bool Settings::hd_objects           = false;

int Settings::hd_objects_size = 3;
