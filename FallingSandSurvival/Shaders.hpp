
#include <SDL2/SDL.h>
#include "SDL_gpu.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Based on https://github.com/grimfang4/sdl-gpu/blob/master/demos/simple-shader/main.c (MIT License)

class Shaders {
public:

    // Loads a shader and prepends version/compatibility info before compiling it.
    static Uint32 load_shader(GPU_ShaderEnum shader_type, const char* filename) {
        SDL_RWops* rwops;
        Uint32 shader;
        char* source;
        int header_size, file_size;
        const char* header = "";
        GPU_Renderer* renderer = GPU_GetCurrentRenderer();

        // Open file
        rwops = SDL_RWFromFile(filename, "rb");
        if(rwops == NULL) {
            GPU_PushErrorCode("load_shader", GPU_ERROR_FILE_NOT_FOUND, "Shader file \"%s\" not found", filename);
            return 0;
        }

        // Get file size
        file_size = SDL_RWseek(rwops, 0, SEEK_END);
        SDL_RWseek(rwops, 0, SEEK_SET);

        // Get size from header
        if(renderer->shader_language == GPU_LANGUAGE_GLSL) {
            if(renderer->max_shader_version >= 120)
                header = "#version 120\n";
            else
                header = "#version 110\n";  // Maybe this is good enough?
        } else if(renderer->shader_language == GPU_LANGUAGE_GLSLES)
            header = "#version 100\nprecision mediump int;\nprecision mediump float;\n";

        header_size = (int)strlen(header);

        // Allocate source buffer
        source = (char*)malloc(sizeof(char) * (header_size + file_size + 1));
        if(source == NULL) throw std::runtime_error("Failed to allocate memory for shader");

        // Prepend header
        #pragma warning(push)
        #pragma warning(disable : 6386)
        strcpy(source, header);
        #pragma warning(pop)

        // Read in source code
        SDL_RWread(rwops, source + strlen(source), 1, file_size);
        source[header_size + file_size] = '\0';

        // Compile the shader
        shader = GPU_CompileShader(shader_type, source);

        // Clean up
        free(source);
        SDL_RWclose(rwops);

        return shader;
    }

    static GPU_ShaderBlock load_shader_program(Uint32* p, const char* vertex_shader_file, const char* fragment_shader_file) {
        Uint32 v, f;
        v = load_shader(GPU_VERTEX_SHADER, vertex_shader_file);

        if(!v)
            GPU_LogError("Failed to load vertex shader (%s): %s\n", vertex_shader_file, GPU_GetShaderMessage());

        f = load_shader(GPU_FRAGMENT_SHADER, fragment_shader_file);

        if(!f)
            GPU_LogError("Failed to load fragment shader (%s): %s\n", fragment_shader_file, GPU_GetShaderMessage());

        *p = GPU_LinkShaders(v, f);

        if(!*p) {
            GPU_ShaderBlock b = {-1, -1, -1, -1};
            GPU_LogError("Failed to link shader program (%s + %s): %s\n", vertex_shader_file, fragment_shader_file, GPU_GetShaderMessage());
            return b;
        }

        {
            GPU_ShaderBlock block = GPU_LoadShaderBlock(*p, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
            GPU_ActivateShaderProgram(*p, &block);

            return block;
        }
    }

    static void free_shader(Uint32 p) {
        GPU_FreeShaderProgram(p);
    }

    static void prepare_water_shader(Uint32 shader, GPU_Target* screen, GPU_Image* image) {
        if(image != NULL) {
            float res[2] = {(float)screen->w, (float)screen->h};
            GPU_SetUniformfv(GPU_GetUniformLocation(shader, "resolution"), 2, 1, res);
        }
    }

    // Will change every frame

    static void update_water_shader(Uint32 shader, float t, int w, int h, GPU_Image* img, int mask_x, int mask_y, int mask_w, int mask_h, int scale) {
        int time_loc = GPU_GetUniformLocation(shader, "time");
        int res_loc = GPU_GetUniformLocation(shader, "resolution");
        int mask_loc = GPU_GetUniformLocation(shader, "mask");
        int mask_pos_loc = GPU_GetUniformLocation(shader, "maskPos");
        int mask_size_loc = GPU_GetUniformLocation(shader, "maskSize");
        int scale_loc = GPU_GetUniformLocation(shader, "scale");

        GPU_SetUniformf(time_loc, t);

        float res[2] = {(float)w, (float)h};
        GPU_SetUniformfv(res_loc, 2, 1, res);

        GPU_SetShaderImage(img, mask_loc, 1);

        float res2[2] = {(float)mask_x, (float)mask_y};
        GPU_SetUniformfv(mask_pos_loc, 2, 1, res2);
        float res3[2] = {(float)mask_w, (float)mask_h};
        GPU_SetUniformfv(mask_size_loc, 2, 1, res3);

        GPU_SetUniformf(scale_loc, scale);

    }

};

class Shader {
public:
    Uint32 shader;
    GPU_ShaderBlock block;

    Shader(const char* vertex_shader_file, const char* fragment_shader_file) {
        shader = 0;
        block = Shaders::load_shader_program(&shader, vertex_shader_file, fragment_shader_file);
    }

    virtual void prepare() = 0;

    void activate() {
        GPU_ActivateShaderProgram(shader, &block);
    }
};

class RaycastLightingShader : public Shader {
public:
    RaycastLightingShader() : Shader("data/shaders/common.vert", "data/shaders/lighting.frag") {};

    void prepare() {}

    void update(GPU_Image* img, float x, float y) {
        int t0_loc = GPU_GetUniformLocation(shader, "t0");
        int txrmap_loc = GPU_GetUniformLocation(shader, "txrmap");

        float res[2] = {x, y};
        GPU_SetUniformfv(t0_loc, 2, 1, res);

        GPU_SetShaderImage(img, txrmap_loc, 1);
    }
};

class SimpleLightingShader : public Shader {
public:
    SimpleLightingShader() : Shader("data/shaders/common.vert", "data/shaders/simpleLighting.frag") {};

    void prepare() {}

    void update(GPU_Image* img, float x, float y) {
        int txrmap_loc = GPU_GetUniformLocation(shader, "txrmap");
        int txrsize_loc = GPU_GetUniformLocation(shader, "texSize");
        int lights_loc = GPU_GetUniformLocation(shader, "lightPoints");
        int nlights_loc = GPU_GetUniformLocation(shader, "nLights");

        float tres[2] = {(float)img->w, (float)img->h};
        GPU_SetUniformfv(txrsize_loc, 2, 1, tres);

        float light[4] = {x * img->w, y * img->h, 200, 200};
        GPU_SetUniformfv(lights_loc, 2, 2, light);

        int nLights = 1;
        GPU_SetUniformi(nlights_loc, nLights);

        GPU_SetShaderImage(img, txrmap_loc, 1);
    }
};

class SimpleLighting2Shader : public Shader {
public:
    SimpleLighting2Shader() : Shader("data/shaders/common.vert", "data/shaders/simpleLighting2.frag") {};

    void prepare() {}

    void update(GPU_Image* tex, GPU_Image* light, float x, float y) {
        int txrmap_loc = GPU_GetUniformLocation(shader, "txrmap");
        int lgtmap_loc = GPU_GetUniformLocation(shader, "lightmap");
        int txrsize_loc = GPU_GetUniformLocation(shader, "texSize");

        float tres[2] = {(float)tex->w, (float)tex->h};
        GPU_SetUniformfv(txrsize_loc, 2, 1, tres);

        GPU_SetShaderImage(tex, txrmap_loc, 1);
        GPU_SetShaderImage(light, lgtmap_loc, 2);
    }
};

class FireShader : public Shader {
public:
    FireShader() : Shader("data/shaders/common.vert", "data/shaders/fire.frag") {};

    void prepare() {}

    void update(GPU_Image* tex) {
        int firemap_loc = GPU_GetUniformLocation(shader, "firemap");
        int txrsize_loc = GPU_GetUniformLocation(shader, "texSize");

        float tres[2] = {(float)tex->w, (float)tex->h};
        GPU_SetUniformfv(txrsize_loc, 2, 1, tres);

        GPU_SetShaderImage(tex, firemap_loc, 1);
    }
};

class Fire2Shader : public Shader {
public:
    Fire2Shader() : Shader("data/shaders/common.vert", "data/shaders/fire2.frag") {};

    void prepare() {}

    void update(GPU_Image* tex) {
        int firemap_loc = GPU_GetUniformLocation(shader, "firemap");
        int txrsize_loc = GPU_GetUniformLocation(shader, "texSize");

        float tres[2] = {(float)tex->w, (float)tex->h};
        GPU_SetUniformfv(txrsize_loc, 2, 1, tres);

        GPU_SetShaderImage(tex, firemap_loc, 1);
    }
};
