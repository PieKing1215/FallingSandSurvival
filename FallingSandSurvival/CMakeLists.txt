set(PROJECT_NAME FallingSandSurvival)

# Source groups

set(no_group_source_files
    ".editorconfig"
    "cpp.hint"
    "resource.h"
)
source_group("" FILES ${no_group_source_files})

set(Source_Files
    "Controls.cpp"
    "Controls.hpp"
    "Game.cpp"
    "Game.hpp"
    "Macros.hpp"
    "main.cpp"
    "ProfilerConfig.hpp"
    "Settings.cpp"
    "Settings.hpp"
    "stdafx.cpp"
    "stdafx.h"
)
source_group("Source Files" FILES ${Source_Files})

set(Source_Files__integration
    "DiscordIntegration.cpp"
    "DiscordIntegration.hpp"
    "DiscordUtils.hpp"
)
source_group("Source Files\\integration" FILES ${Source_Files__integration})

set(Source_Files__libraries__audioAdventure
    "lib/AudioAdventure/include/AudioEngine.h"
    "lib/AudioAdventure/source/AudioEngine.cpp"
)
source_group("Source Files\\libraries\\audioAdventure" FILES ${Source_Files__libraries__audioAdventure})

set(Source_Files__libraries__ctpl-threadpool
    "lib/CTPL-ctpl_v.0.0.2/ctpl_stl.h"
)
source_group("Source Files\\libraries\\ctpl-threadpool" FILES ${Source_Files__libraries__ctpl-threadpool})

set(Source_Files__libraries__discord-game-sdk
    "lib/discord_game_sdk/cpp/achievement_manager.cpp"
    "lib/discord_game_sdk/cpp/achievement_manager.h"
    "lib/discord_game_sdk/cpp/activity_manager.cpp"
    "lib/discord_game_sdk/cpp/activity_manager.h"
    "lib/discord_game_sdk/cpp/application_manager.cpp"
    "lib/discord_game_sdk/cpp/application_manager.h"
    "lib/discord_game_sdk/cpp/core.cpp"
    "lib/discord_game_sdk/cpp/core.h"
    "lib/discord_game_sdk/cpp/discord.h"
    "lib/discord_game_sdk/cpp/event.h"
    "lib/discord_game_sdk/cpp/ffi.h"
    "lib/discord_game_sdk/cpp/image_manager.cpp"
    "lib/discord_game_sdk/cpp/image_manager.h"
    "lib/discord_game_sdk/cpp/lobby_manager.cpp"
    "lib/discord_game_sdk/cpp/lobby_manager.h"
    "lib/discord_game_sdk/cpp/network_manager.cpp"
    "lib/discord_game_sdk/cpp/network_manager.h"
    "lib/discord_game_sdk/cpp/overlay_manager.cpp"
    "lib/discord_game_sdk/cpp/overlay_manager.h"
    "lib/discord_game_sdk/cpp/relationship_manager.cpp"
    "lib/discord_game_sdk/cpp/relationship_manager.h"
    "lib/discord_game_sdk/cpp/storage_manager.cpp"
    "lib/discord_game_sdk/cpp/storage_manager.h"
    "lib/discord_game_sdk/cpp/store_manager.cpp"
    "lib/discord_game_sdk/cpp/store_manager.h"
    "lib/discord_game_sdk/cpp/types.cpp"
    "lib/discord_game_sdk/cpp/types.h"
    "lib/discord_game_sdk/cpp/user_manager.cpp"
    "lib/discord_game_sdk/cpp/user_manager.h"
    "lib/discord_game_sdk/cpp/voice_manager.cpp"
    "lib/discord_game_sdk/cpp/voice_manager.h"
)
source_group("Source Files\\libraries\\discord-game-sdk" FILES ${Source_Files__libraries__discord-game-sdk})

set(Source_Files__libraries__douglas-peucker
    "lib/douglas-peucker/polygon-simplify.cc"
    "lib/douglas-peucker/polygon-simplify.hh"
)
source_group("Source Files\\libraries\\douglas-peucker" FILES ${Source_Files__libraries__douglas-peucker})

set(Source_Files__libraries__fastNoise
    "lib/FastNoise/FastNoise.cpp"
    "lib/FastNoise/FastNoise.h"
)
source_group("Source Files\\libraries\\fastNoise" FILES ${Source_Files__libraries__fastNoise})

set(Source_Files__libraries__fastNoiseSIMD
    "lib/FastNoiseSIMD/FastNoiseSIMD.cpp"
    "lib/FastNoiseSIMD/FastNoiseSIMD.h"
    "lib/FastNoiseSIMD/FastNoiseSIMD_avx2.cpp"
    "lib/FastNoiseSIMD/FastNoiseSIMD_avx512.cpp"
    "lib/FastNoiseSIMD/FastNoiseSIMD_internal.cpp"
    "lib/FastNoiseSIMD/FastNoiseSIMD_internal.h"
    "lib/FastNoiseSIMD/FastNoiseSIMD_neon.cpp"
    "lib/FastNoiseSIMD/FastNoiseSIMD_sse2.cpp"
    "lib/FastNoiseSIMD/FastNoiseSIMD_sse41.cpp"
)
source_group("Source Files\\libraries\\fastNoiseSIMD" FILES ${Source_Files__libraries__fastNoiseSIMD})

set(Source_Files__libraries__imgui
    "lib/imgui/backends/imgui_impl_opengl3.cpp"
    "lib/imgui/backends/imgui_impl_opengl3.h"
    "lib/imgui/backends/imgui_impl_sdl.cpp"
    "lib/imgui/backends/imgui_impl_sdl.h"
    "lib/imgui/libs/gl3w/GL/gl3w.c"
    "lib/imgui/libs/gl3w/GL/gl3w.h"
    "lib/imgui/libs/gl3w/GL/glcorearb.h"
)
source_group("Source Files\\libraries\\imgui" FILES ${Source_Files__libraries__imgui})

set(Source_Files__libraries__marching-squares
    "lib/cpp-marching-squares-master/Direction.h"
    "lib/cpp-marching-squares-master/MarchingSquares.h"
    "lib/cpp-marching-squares-master/Utility.h"
)
source_group("Source Files\\libraries\\marching-squares" FILES ${Source_Files__libraries__marching-squares})

set(Source_Files__libraries__polypartition
    "lib/polypartition-master/src/polypartition.cpp"
    "lib/polypartition-master/src/polypartition.h"
)
source_group("Source Files\\libraries\\polypartition" FILES ${Source_Files__libraries__polypartition})

set(Source_Files__libraries__sparsehash
    "lib/sparsehash/dense_hash_map.h"
    "lib/sparsehash/dense_hash_set"
    "lib/sparsehash/internal/densehashtable.h"
    "lib/sparsehash/internal/hashtable-common.h"
    "lib/sparsehash/internal/libc_allocator_with_realloc.h"
    "lib/sparsehash/internal/sparsehashtable.h"
    "lib/sparsehash/sparse_hash_map.h"
    "lib/sparsehash/sparse_hash_set"
    "lib/sparsehash/sparsetable"
    "lib/sparsehash/traits"
)
source_group("Source Files\\libraries\\sparsehash" FILES ${Source_Files__libraries__sparsehash})

set(Source_Files__net
    "Networking.cpp"
    "Networking.hpp"
)
source_group("Source Files\\net" FILES ${Source_Files__net})

set(Source_Files__objects
    "Entity.cpp"
    "Entity.hpp"
    "Item.cpp"
    "Item.hpp"
    "Particle.cpp"
    "Particle.hpp"
    "Player.cpp"
    "Player.hpp"
    "RigidBody.cpp"
    "RigidBody.hpp"
)
source_group("Source Files\\objects" FILES ${Source_Files__objects})

set(Source_Files__util
    "CLArgs.hpp"
    "GameDir.cpp"
    "GameDir.hpp"
    "UTime.cpp"
    "UTime.hpp"
)
source_group("Source Files\\util" FILES ${Source_Files__util})

set(Source_Files__vfx
    "Drawing.cpp"
    "Drawing.hpp"
    "Shaders.hpp"
    "Textures.cpp"
    "Textures.hpp"
    "b2DebugDraw_impl.cpp"
    "b2DebugDraw_impl.hpp"
)
source_group("Source Files\\vfx" FILES ${Source_Files__vfx})

set(Source_Files__vfx__gui
    "CreateWorldUI.cpp"
    "DebugCheatsUI.cpp"
    "DebugDrawUI.cpp"
    "DebugUI.cpp"
    "IngameUI.cpp"
    "MainMenuUI.cpp"
    "OptionsUI.cpp"
    "UIs.hpp"
)
source_group("Source Files\\vfx\\gui" FILES ${Source_Files__vfx__gui})

set(Source_Files__world
    "Background.cpp"
    "Background.hpp"
    "Biome.cpp"
    "Biome.hpp"
    "Chunk.cpp"
    "Chunk.hpp"
    "ChunkReadyToMerge.hpp"
    "world.cpp"
    "world.hpp"
)
source_group("Source Files\\world" FILES ${Source_Files__world})

set(Source_Files__world__generation
    "DefaultGenerator.cpp"
    "MaterialTestGenerator.cpp"
    "Populator.cpp"
    "Populator.hpp"
    "Populators.cpp"
    "WorldGenerator.hpp"
)
source_group("Source Files\\world\\generation" FILES ${Source_Files__world__generation})

set(Source_Files__world__materials
    "Material.cpp"
    "Material.hpp"
    "MaterialInstance.cpp"
    "MaterialInstance.hpp"
    "Materials.cpp"
    "Materials.hpp"
    "PhysicsType.hpp"
    "Tiles.cpp"
    "Tiles.hpp"
)
source_group("Source Files\\world\\materials" FILES ${Source_Files__world__materials})

set(Source_Files__world__structures
    "PlacedStructure.cpp"
    "PlacedStructure.hpp"
    "Structure.cpp"
    "Structure.hpp"
    "Structures.cpp"
    "Structures.hpp"
)
source_group("Source Files\\world\\structures" FILES ${Source_Files__world__structures})

set(ALL_FILES
    ${no_group_source_files}
    ${Source_Files}
    ${Source_Files__integration}
    ${Source_Files__libraries__audioAdventure}
    ${Source_Files__libraries__ctpl-threadpool}
    #${Source_Files__libraries__discord-game-sdk}
    ${Source_Files__libraries__douglas-peucker}
    ${Source_Files__libraries__fastNoise}
    ${Source_Files__libraries__fastNoiseSIMD}
    ${Source_Files__libraries__imgui}
    ${Source_Files__libraries__marching-squares}
    ${Source_Files__libraries__polypartition}
    ${Source_Files__libraries__sparsehash}
    ${Source_Files__net}
    ${Source_Files__objects}
    ${Source_Files__util}
    ${Source_Files__vfx}
    ${Source_Files__vfx__gui}
    ${Source_Files__world}
    ${Source_Files__world__generation}
    ${Source_Files__world__materials}
    ${Source_Files__world__structures}
)

################################################################################
# Target
################################################################################

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

set(SPDLOG_FMT_EXTERNAL ON)

message("=================================")
message("=================================")
message("= STARTING CONAN                =")
message("=================================")
message("=================================")
include(${CMAKE_BINARY_DIR}/conan.cmake)

if(WINDOWS)
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP 
                BUILD missing SETTINGS)
elseif(LINUX)
conan_cmake_run(CONANFILE conanfile_linux.txt
                BASIC_SETUP 
                BUILD missing)
endif()
                
message("=================================")
message("=================================")
message("= CONAN DONE                    =")
message("=================================")
message("=================================")

message("CONAN_INCLUDE_DIRS_DEBUG = ${CONAN_INCLUDE_DIRS_DEBUG}")
include_directories(${CONAN_INCLUDE_DIRS_DEBUG})

message(STATUS "Conan: Loading ${_CONANBUILDINFOFOLDER}/${_CONANBUILDINFO}")

#include(${_CONANBUILDINFOFOLDER}/conanbuildinfo_multi.cmake)

#set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /LIBPATH:${CONAN_LIB_DIRS_DEBUG}")
#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LIBPATH:${CONAN_LIB_DIRS_RELEASE}")

#link_directories(${CONAN_LIB_DIRS_DEBUG})
#link_directories(${CONAN_LIB_DIRS_RELEASE})

set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(CMAKE_INSTALL_RPATH "$ORIGIN/lib;$ORIGIN/../../FallingSandSurvival/lib/FMOD/lib/x64;$ORIGIN/../../FallingSandSurvival/lib/FMOD_studio/lib/x64")
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(CMAKE_INSTALL_RPATH "$ORIGIN/lib;$ORIGIN/../../FallingSandSurvival/lib/FMOD/lib/x86;$ORIGIN/../../FallingSandSurvival/lib/FMOD_studio/lib/x86")
endif()
add_executable(${PROJECT_NAME} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH_USE_ORIGIN ON)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
set_source_files_properties("lib/FastNoiseSIMD/FastNoiseSIMD_sse2.cpp" PROPERTIES COMPILE_FLAGS -msse2)
set_source_files_properties("lib/FastNoiseSIMD/FastNoiseSIMD_sse41.cpp" PROPERTIES COMPILE_FLAGS -msse4.1)
set_source_files_properties("lib/FastNoiseSIMD/FastNoiseSIMD_avx2.cpp" PROPERTIES COMPILE_FLAGS -mavx2)
set_source_files_properties("lib/FastNoiseSIMD/FastNoiseSIMD_avx512.cpp" PROPERTIES COMPILE_FLAGS -mavx512f)

target_precompile_headers(${PROJECT_NAME} PUBLIC "stdafx.h")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE FallingSandSurvival)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Release>:
            MultiThreadedDLL
        >
        $<$<NOT:$<OR:$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Release>:
            MultiThreadedDLL
        >
        $<$<NOT:$<OR:$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD/inc;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD_studio/inc;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/steam/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/discord_game_sdk/cpp;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/libs/gl3w"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD/inc;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD_studio/inc;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/steam/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/discord_game_sdk/cpp;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends;"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/libs/gl3w"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "IMGUI_IMPL_OPENGL_LOADER_GL3W;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_MBCS;"
        "_WINSOCK_DEPRECATED_NO_WARNINGS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "IMGUI_IMPL_OPENGL_LOADER_GL3W;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_MBCS;"
        "_WINSOCK_DEPRECATED_NO_WARNINGS"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    message("########## MSVC")
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Gy
            >
            /permissive-;
            /Oi;
            /sdl;
            /Oy-;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /Ot;
            /TP;
            /wd4244;
            /wd4101;
            /wd4018;
            /wd4305;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /GL-;
                /Od;
                /Oy-
            >
            $<$<CONFIG:Release>:
                /O2;
                /Gy
            >
            /permissive-;
            /MP;
            /Oi;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /Ot;
            /TP;
            /wd4244;
            /wd4101;
            /wd4018;
            /wd4305;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG:FULL;
            /SUBSYSTEM:CONSOLE;
            /ignore:4099
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG:FULL;
            /SUBSYSTEM:CONSOLE;
            /ignore:4099
        )
    endif()
else()    
    message("########## UNIX")
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        message("########## x64")
        target_compile_options(${PROJECT_NAME} PUBLIC -O2)
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        message("########## x86")
        target_compile_options(${PROJECT_NAME} PUBLIC -O2)
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                -O0
            >
            $<$<CONFIG:Release>:
                -O2
            >
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                -O0
            >
            $<$<CONFIG:Release>:
                -O2
            >
        )
    endif()
endif()

################################################################################
# Pre build events
################################################################################

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        PRE_BUILD
        COMMANDS
        COMMAND   $<CONFIG:Debug> del $<CONFIG>\\Game.obj || exit 0
        COMMAND $<CONFIG:Release> del $<CONFIG>\\Game.obj || exit 0
        COMMENT "
          "
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        PRE_BUILD
        COMMANDS
        COMMAND   $<CONFIG:Debug> del $<CONFIG>\\Game.obj || exit 0
        COMMAND $<CONFIG:Release> del $<CONFIG>\\Game.obj || exit 0
        COMMENT "
          "
    )
endif()


message("1")

################################################################################
# Dependencies
################################################################################
message("CMAKE_VS_PLATFORM_NAME = ${CMAKE_VS_PLATFORM_NAME}")
if(WINDOWS)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        set(ADDITIONAL_LIBRARY_DEPENDENCIES
            "fmodL_vc;"
            "fmodstudioL_vc;"
            "ws2_32;"
            "winmm"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        set(ADDITIONAL_LIBRARY_DEPENDENCIES
            "fmodL_vc;"
            "fmodstudioL_vc;"
            "ws2_32;"
            "winmm"
        )
    endif()

elseif(LINUX)
    
    message("LINUX target_link_libraries")
    message("CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        message("x64")
        
        target_link_directories(${PROJECT_NAME} PRIVATE 
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD/lib/x64"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD_studio/lib/x64")
        
        target_link_libraries(${PROJECT_NAME} "libfmod.so")
        target_link_libraries(${PROJECT_NAME} "libfmodL.so")
        target_link_libraries(${PROJECT_NAME} "libfmodstudio.so")
        target_link_libraries(${PROJECT_NAME} "libfmodstudioL.so")
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        message("x86")
        
        target_link_directories(${PROJECT_NAME} PRIVATE 
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD/lib/x86"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD_studio/lib/x86")
        
        target_link_libraries(${PROJECT_NAME} "libfmod.so")
        target_link_libraries(${PROJECT_NAME} "libfmodL.so")
        target_link_libraries(${PROJECT_NAME} "libfmodstudio.so")
        target_link_libraries(${PROJECT_NAME} "libfmodstudioL.so")
    endif()
    get_target_property(OUT ${PROJECT_NAME} LINK_LIBRARIES)
    message("LINK_LIBRARIES = ${OUT}")
    
    target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()

message("2")
conan_target_link_libraries(${PROJECT_NAME})
message("3")
if(WINDOWS)
    target_link_libraries(${PROJECT_NAME} "${ADDITIONAL_LIBRARY_DEPENDENCIES}" )
endif()

message("4")
if(WINDOWS)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_directories(${PROJECT_NAME} PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD/lib/x64;"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD_studio/lib/x64;"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/steam/redistributable_bin/win64;"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/discord_game_sdk/lib/x86_64"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_directories(${PROJECT_NAME} PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD/lib/x86;"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD_studio/lib/x86;"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/steam/redistributable_bin;"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/discord_game_sdk/lib/x86"
        )
    endif()
endif()
message("5")
get_target_property(OUT ${PROJECT_NAME} LINK_LIBRARIES)
message("LINK_LIBRARIES = ${OUT}")

