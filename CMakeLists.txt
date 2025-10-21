cmake_minimum_required(VERSION 3.27)
project(game-template CXX)

build_application(
    SOURCES
    Application.cpp
    game_world.cpp
    main_scene.cpp
    sound.cpp
    editor_panels.cpp

    PACKAGES
    spdlog
    flecs
    Jolt
    miniaudio
    atlas

    LINK_PACKAGES
    spdlog::spdlog
    flecs::flecs_static
    Jolt::Jolt
    miniaudio::miniaudio
    atlas::atlas
)

generate_compile_commands()
