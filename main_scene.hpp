#pragma once
#include <core/core.hpp>
#include <core/scene/scene.hpp>
#include <core/scene/scene_object.hpp>
#include <string>
#include <vector>
#include <core/serialize/serializer.hpp>
#include <physics/physics_engine.hpp>
#include <core/event/event_bus.hpp>
#include <core/event/types.hpp>
#include "editor_panels.hpp"
#include "sound.hpp"

/**
 * @name main_scene
 * @brief Implementation of a custom scene that contains game objects
 * 
 * main_scene acts as the first scene created that is associated with the world.
 * 
 */
class main_scene : public atlas::scene_scope {
public:
    main_scene(const std::string& p_tag, atlas::event::event_bus& p_bus);

    ~main_scene() {
        // m_testing_sound_source.stop();
        // m_testing_sound_source.cleanup();
    }


    void start_game();

    //! @note Remove these from being overridden
    //! TODO: We should have an indication on what functions are update phased
    //! functions
    void on_update();

    void on_ui_update();

    //! TODO: Remove this and integrate a few functions into LevelScene such as
    //! on_runtime_start/on_runtime_stop
    void on_physics_update();


    void collision_enter(atlas::event::collision_enter& p_event);

    void collision_persisted(atlas::event::collision_persisted& p_event);

    void collision_removed(atlas::event::collision_exit& p_event);


private:
    // TODO: Will implement scene management system to coordinate with physics system
    //      for starting and stopping the physics runtime
    void runtime_start();
    void runtime_stop();

    // All this does is resets all of the game objects back to their initial starting positions
    // NOTE: Typically re-serialization would occur in replacement of this
    void reset_objects();

    void respawn();

private:
    atlas::serializer m_deserializer_test;
    // atlas::optional_ref<atlas::scene_object> m_viking_room;

    // cube stuff
    atlas::optional_ref<atlas::scene_object> m_cube;
    atlas::transform m_cube_initial_transform;
    float m_cube_speed= 8.f;
    bool m_cube_moving = false;
    bool m_cube_charging=false;
    bool m_has_charged=false;
    float m_cube_trigger_distance=5.f;
    float m_respawn_distance = 20.f;
    float m_cube_miss_distance = 30.f;




    atlas::optional_ref<atlas::scene_object> m_platform;
    atlas::optional_ref<atlas::scene_object> m_border1;
    atlas::optional_ref<atlas::scene_object> m_border2;
    
    atlas::optional_ref<atlas::scene_object> m_sphere;
    atlas::optional_ref<atlas::scene_object> m_box;

    atlas::optional_ref<atlas::scene_object> m_camera;
    atlas::optional_ref<atlas::scene_object> m_runtime_camera;
    atlas::physics::physics_engine m_physics_engine_handler;

    editor_panel m_panels;
    // sound_test m_play_sound;
    ma_engine m_audio_engine;
    std::thread m_audio_thread;
    std::mutex m_audio_mutex;
    std::atomic<bool> m_is_audio_playing=false;
    std::atomic<bool> m_stop_audio_thread=false;

    bool m_blink_text=false;
    glm::vec3 m_offset_from_camera;

    bool m_physics_is_runtime=false;

};