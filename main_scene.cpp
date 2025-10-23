#include "main_scene.hpp"
#include <core/common.hpp>
#include <core/math/utilities.hpp>
#include <core/application.hpp>
#include <core/event/event.hpp>
#include <drivers/jolt-cpp/jolt_components.hpp>
#include <any>
#include <thread>
#include <chrono>

main_scene::main_scene(const std::string& p_tag, atlas::event::event_bus& p_bus)
  : atlas::scene_scope(p_tag, p_bus) {

    m_camera = create_object("camera");

    // Editor camera creation
    m_camera->add<flecs::pair<atlas::tag::editor, atlas::projection_view>>();
    m_camera->set<atlas::transform>({
        .position = { 2.46f, 4.90f, -34.92f },
        .rotation = {0.f, 3.08f, 0.f},
        .scale{ 1.f },
    });
    m_camera->set<atlas::perspective_camera>({
      .plane = { 0.1f, 5000.f },
      .is_active = true,
      .field_of_view = 45.f,
    });
    m_camera->add<atlas::tag::serialize>();

    m_runtime_camera = create_object("game camera");
    m_runtime_camera->add<flecs::pair<atlas::tag::editor, atlas::projection_view>>();
    m_runtime_camera->set<atlas::transform>({
        .position = { 2.46f, 4.90f, -34.92f },
        .rotation = {0.f, 3.08f, 0.f},
        .scale{ 1.f },
    });
    m_runtime_camera->set<atlas::perspective_camera>({
      .plane = { 0.1f, 5000.f },
      .is_active = false,
      .field_of_view = 45.f,
    });
    m_runtime_camera->add<atlas::tag::serialize>();


    // creating cube
    m_cube = create_object("Cube");
    m_cube->add<atlas::tag::serialize>();
    m_cube->set<atlas::transform>({
        .position = {7.20f, 2.70f, -1.70f},
        .rotation = {0.10f, 1.55f, 7.96f},
        .scale{6.f},
    });
    m_cube->set<atlas::material>({
        .color = {0.f, 1.f, 0.f, 1.f}, // R, G, B, A
        .model_path = "assets/models/cube.obj",
        .texture_path = "assets/models/Tiles074_8K-JPG_Color.jpg"
    });
    m_cube->add<atlas::tag::serialize>();

    m_platform = create_object("Platform");

    m_platform->set<atlas::transform>({
        .scale = { 15.f, 0.30f, 10.0f },
    });

    m_platform->add<atlas::tag::serialize>();

    m_platform->set<atlas::material>({
        .color = {1.f, 1.f, 1.f, 1.f},
        .model_path = "assets/models/cube.obj",
        .texture_path = "assets/models/wood.png"
    });
    
    m_platform->set<atlas::physics_body>({
      .body_movement_type = atlas::fixed,
    });
    m_platform->set<atlas::box_collider>({
      .half_extent = { 15.f, 0.30f, 10.0f },
    });

    m_sphere = create_object("Sphere");
    m_sphere->add<atlas::tag::serialize>();

    m_sphere->set<atlas::transform>({
        .position = {-2.70f, 2.70, -8.30f},
        .rotation = {2.30f, 95.90f, 91.80f},
        .scale = {1.f, 1.f, 1.f},
    });
    m_sphere->set<atlas::material>({
        .color{1.f},
        .model_path = "assets/models/Ball OBJ.obj",
        .texture_path = "assets/models/clear.png"
    });

    // Adding physics body
    m_sphere->set<atlas::physics_body>({
        .body_movement_type = atlas::dynamic
    });

    glm::vec3 sphere_scale(1.f);
    m_sphere->set<atlas::sphere_collider>({
        .radius = glm::compMax(sphere_scale) * 2.5f,
    });
    m_sphere->set<atlas::transform>({
        .position = {-2.70f, 2.70, -8.30f},
        .scale{1.f}
    });

    m_border1 = create_object("Border 1");
    m_border1->set<atlas::transform>({
        .scale = { 15.f, 0.30f, 10.0f },
    });

    m_border1->add<atlas::tag::serialize>();

    m_border1->set<atlas::material>({
        .color = {1.f, 1.f, 1.f, 1.f},
        .model_path = "assets/models/cube.obj",
        .texture_path = "assets/models/wood.png"
    });
    
    m_border1->set<atlas::physics_body>({
      .body_movement_type = atlas::fixed,
    });
    m_border1->set<atlas::box_collider>({
      .half_extent = { 15.f, 0.30f, 10.0f },
    });

    m_border1->add<atlas::tag::serialize>();

    m_border2 = create_object("Border 2");

    m_border2->set<atlas::transform>({
        .scale = { 15.f, 0.30f, 10.0f },
    });

    m_border2->add<atlas::tag::serialize>();

    m_border2->set<atlas::material>({
        .color = {1.f, 1.f, 1.f, 1.f},
        .model_path = "assets/models/cube.obj",
        .texture_path = "assets/models/wood.png"
    });
    
    m_border2->set<atlas::physics_body>({
      .body_movement_type = atlas::fixed,
    });
    m_border2->set<atlas::box_collider>({
      .half_extent = { 15.f, 0.30f, 10.0f },
    });

    m_border2->add<atlas::tag::serialize>();

    // subscription example
    subscribe<atlas::event::collision_enter>(this, &main_scene::collision_enter);
    subscribe<atlas::event::collision_persisted>(this, &main_scene::collision_persisted);


    // game state behavior
    // registration update callbacks for offloading your own game logic
    atlas::register_start(this, &main_scene::start_game);
    atlas::register_update(this, &main_scene::on_update);
    atlas::register_physics(this, &main_scene::on_physics_update);
    atlas::register_ui(this, &main_scene::on_ui_update);
}

void main_scene::reset_objects() {

    // m_sphere->set<atlas::transform>({
    //     .position = {-2.70f, 2.70, -8.30f},
    //     .scale{1.f}
    // });

    // glm::vec3 sphere_scale{5.f};
    // m_sphere->set<atlas::transform>({
    //     .position = {-2.70f, 2.70, -8.30f},
    //     .rotation = {2.30f, 95.90f, 91.80f},
    //     .scale = sphere_scale,
    // });
    
    m_deserializer_test = atlas::serializer();
    if(!m_deserializer_test.load("LevelScene", *this)) {
        console_log_error("Cannot load LevelScene!!!");
    }



}


void edit_transform(atlas::transform* p_transform) {
    atlas::ui::draw_vec3("Position", p_transform->position);
    atlas::ui::draw_vec3("Scale", p_transform->scale);
    atlas::ui::draw_vec3("Rotation", p_transform->rotation);
}

void play_sound(ma_engine* pEngine, const std::string& filename) {
    if (!pEngine) {
        return;
    }

    ma_result result;
    ma_sound sound;

    result = ma_sound_init_from_file(pEngine, filename.c_str(), 0, nullptr, nullptr, &sound);
    if (result != MA_SUCCESS) {
        return;
    }

    result = ma_sound_start(&sound);
    if (result != MA_SUCCESS) {
        ma_sound_uninit(&sound);
        return;
    }

    while (ma_sound_is_playing(&sound)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ma_sound_uninit(&sound);
}

void main_scene::collision_enter(atlas::event::collision_enter& p_event) {
    // console_log_warn("Collision Enter happened!!! Executed from main_scene::collision_enter"); 
    // console_log_info("main_scene::collision_enter happened!");

    // flecs::world registry = *this;
    // flecs::entity e1 = registry.entity(p_event.entity1);
    // flecs::entity e2 = registry.entity(p_event.entity2);

    // console_log_info("e1 = {}", e1.name().c_str());
    // console_log_info("e2 = {}", e2.name().c_str());
}

void main_scene::collision_persisted(atlas::event::collision_persisted& p_event) {
    flecs::world registry = *this;
    
    flecs::entity e1 = registry.entity(p_event.entity1);
    flecs::entity e2 = registry.entity(p_event.entity2);

    if(e2.name() == "Sphere") {
        // This will only ever play the audio whenever the ball has made contact with the platform
        /*
        std::string sound_file = "Resources/rolling_ball_on_wood.mp3";
        // if(!m_is_audio_playing.load()) {
        std::lock_guard<std::mutex> lock(m_audio_mutex);
        m_is_audio_playing = true;

        // We create a separate thread that runs in the background
        m_audio_thread = std::thread([this, sound_file](){
            play_sound(&m_audio_engine, sound_file);
            m_is_audio_playing=false;
        });

        // we detach this audio thread from the game-thread (main) to play in the background
        m_audio_thread.detach();
        // }
        */
    }
}

void main_scene::start_game() {
    // we just initialize the audio engine -- I am just doing this for funsies and experiementation
    ma_result res = ma_engine_init(nullptr, &m_audio_engine);

    if(res != MA_SUCCESS) {
        console_log_error("Could not initialize ma_engine!!!");
    }
    
    m_deserializer_test = atlas::serializer();
    if(!m_deserializer_test.load("LevelScene", *this)) {
        console_log_error("Cannot load LevelScene!!!");
    }

    m_cube_initial_transform = *m_cube->get<atlas::transform>();
    m_panels = editor_panel(*this, *event_handle());

    atlas::physics::jolt_settings settings = {};
    flecs::world registry = *this;
    m_physics_engine_handler = atlas::physics::physics_engine(settings, registry, *event_handle());
}

void main_scene::runtime_start() {
    // runs the physics simulation
    m_physics_is_runtime = true;

    m_physics_engine_handler.start();
}

void main_scene::runtime_stop() {
    // stops the physics simulation
    // also does post-cleanup
    m_physics_is_runtime = false;

    m_physics_engine_handler.stop();

    reset_objects();
}

void
main_scene::on_ui_update() {
    m_panels.render_properties_panel();

    atlas::ui::draw_float("Trigger Distance", m_cube_trigger_distance);
}

void
main_scene::on_update() {
    float smooth_speed = 0.1f;
    atlas::transform* camera_transform = m_camera->get_mut<atlas::transform>();
    atlas::transform* sphere_transform = m_sphere->get_mut<atlas::transform>();

    float dt = atlas::application::delta_time();
    float movement_speed = 10.f;
    float rotation_speed = 1.f;
    float velocity = movement_speed * dt;
    float rotation_velocity = rotation_speed * dt;
    glm::quat quaternion = atlas::to_quat(camera_transform->quaternion);
    glm::vec3 up = glm::rotate(quaternion, glm::vec3(0.f, 1.f, 0.f));
    glm::vec3 forward = glm::rotate(quaternion, glm::vec3(0.f, 0.f, -1.f));
    glm::vec3 right = glm::rotate(quaternion, glm::vec3(1.0f, 0.0f, 0.0f));

    if (atlas::event::is_key_pressed(key_left_shift)) {
        if (atlas::event::is_mouse_pressed(mouse_button_middle)) {
            camera_transform->position += up * velocity;
        }

        if (atlas::event::is_mouse_pressed(mouse_button_right)) {
            camera_transform->position -= up * velocity;
        }
    }

    if (atlas::event::is_key_pressed(key_w)) {
        camera_transform->position += forward * velocity;
    }
    if (atlas::event::is_key_pressed(key_s)) {
        camera_transform->position -= forward * velocity;
    }

    if (atlas::event::is_key_pressed(key_d)) {
        camera_transform->position += right * velocity;
    }
    if (atlas::event::is_key_pressed(key_a)) {
        camera_transform->position -= right * velocity;
    }

    if (atlas::event::is_key_pressed(key_q)) {
        camera_transform->rotation.y += rotation_velocity;
    }
    if (atlas::event::is_key_pressed(key_e)) {
        camera_transform->rotation.y -= rotation_velocity;
    }

    camera_transform->set_rotation(camera_transform->rotation);
}

void main_scene::respawn() {
    m_cube->set<atlas::transform>(m_cube_initial_transform);
    m_cube_moving = false;
}

void
main_scene::on_physics_update() {
    float dt = atlas::application::delta_time();
    atlas::physics_body* sphere_body = m_sphere->get_mut<atlas::physics_body>();
    atlas::transform* cube_transform = m_cube->get_mut<atlas::transform>();
    atlas::transform* sphere_transform = m_sphere->get_mut<atlas::transform>();
    atlas::perspective_camera* editor_camera = m_camera->get_mut<atlas::perspective_camera>();
    atlas::perspective_camera* game_camera = m_runtime_camera->get_mut<atlas::perspective_camera>();
    atlas::transform* game_camera_transform = m_runtime_camera->get_mut<atlas::transform>();

    if (atlas::event::is_key_pressed(key_r) and !m_physics_is_runtime) {
        editor_camera->is_active = false;
        game_camera->is_active = true;
        runtime_start();
    }

    if(m_physics_is_runtime) {
        m_physics_engine_handler.update(dt);

        // Calculate direction from cube to sphere
        glm::vec3 direction = sphere_transform->position - cube_transform->position;
        float distance = glm::length(direction);
        
        if (!m_cube_charging && !m_has_charged && distance < m_cube_trigger_distance) {
            m_cube_charging = true;
            m_has_charged = true;
            console_log_warn("Cube is charging at sphere!");
        }
        
        // Charge towards sphere if active
        if (m_cube_charging) {
            // Normalize direction and charge at full speed
            if (distance > 0.1f) {  // Avoid division by zero
                direction = glm::normalize(direction);
                cube_transform->position += direction * m_cube_speed * dt;
            }
            
            // Check if cube missed (got too far from sphere)
            if (distance > m_respawn_distance) {
                console_log_warn("Cube missed! Respawning...");
                respawn();
            }
        }


        if(game_camera->is_active) {
            float camera_look_ahead_offset = 2.f;
            glm::vec3 camera_offset = {0.f, 5.f, 10.f};  // Offset from sphere

            // glm::vec3 target_position = sphere_transform->position + camera_offset;
            glm::vec3 look_ahead = sphere_transform->position + glm::normalize(sphere_body->linear_velocity) * camera_look_ahead_offset;
    
            // Position camera behind sphere
            glm::vec3 camera_direction = glm::normalize(sphere_transform->position - look_ahead);
            glm::vec3 target_position = sphere_transform->position - camera_direction * glm::length(camera_offset);
            target_position.y += camera_offset.y;  // Keep height offset
            
            // Smooth camera movement
            float follow_speed = 3.f;
            game_camera_transform->position = glm::mix(
                game_camera_transform->position, 
                target_position, 
                follow_speed * atlas::application::delta_time()
            );
            
            // Look at sphere
            glm::vec3 direction = glm::normalize(sphere_transform->position - game_camera_transform->position);
            float yaw = glm::atan(direction.x, direction.z);
            float pitch = glm::asin(-direction.y);
            
            game_camera_transform->rotation = {pitch, yaw, 0.f};
            game_camera_transform->set_rotation(game_camera_transform->rotation);
            // float camera_follow_speed = 3.f;
            // game_camera_transform->position = glm::mix(
            //     game_camera_transform->position, 
            //     target_position, 
            //     camera_follow_speed * dt
            // );
            
            // glm::vec3 direction = glm::normalize(sphere_transform->position - game_camera_transform->position);
            // float yaw = glm::atan(direction.x, direction.z);
            // float pitch = glm::asin(-direction.y);
            
            // game_camera_transform->rotation = {pitch, yaw, 0.f};
            // game_camera_transform->set_rotation(game_camera_transform->rotation);
        }
        
        // Check if cube hit the sphere (collision will be handled by collision events)
        if (m_cube_charging && distance < 2.0f) {
            console_log_warn("Cube hit sphere! Respawning...");
            respawn();
        }
    }

    if (atlas::event::is_key_pressed(key_l) and m_physics_is_runtime) {
        runtime_stop();
        editor_camera->is_active = true;
        game_camera->is_active = false;
    }

    // U = +up
    // J = -up
    // H = +left
    // L = -Left
    if(atlas::event::is_key_pressed(key_u)) {
        glm::vec3 angular_vel = {0.f, 1.f, 0.f};
        sphere_body->angular_velocity = angular_vel;
    }

    if(atlas::event::is_key_pressed(key_j)) {
        glm::vec3 angular_vel = {0.f, -1.f, 0.f};
        sphere_body->angular_velocity = angular_vel;
    }

    if(atlas::event::is_key_pressed(key_h)) {
        glm::vec3 angular_vel = {1.f, 0.f, 0.f};
        sphere_body->angular_velocity = angular_vel;
    }

    if(atlas::event::is_key_pressed(key_l)) {
        glm::vec3 angular_vel = {-1.f, 0.f, 0.f};
        sphere_body->angular_velocity = angular_vel;
    }

    if (atlas::event::is_key_pressed(key_space)) {
        glm::vec3 linear_velocity = { 0.f, 10.0f, 0.f };
        sphere_body->linear_velocity = linear_velocity;
    }

    // checking if there is a controller device joystic connected
    bool controller_connected = atlas::event::is_joystic_present(0);

    if(controller_connected) {
        float speed = 10.f;
        float left_joystick_x = atlas::event::get_joystic_axis(0, GLFW_GAMEPAD_AXIS_LEFT_X);
        float left_joystick_y = atlas::event::get_joystic_axis(0, GLFW_GAMEPAD_AXIS_LEFT_Y);
        float right_joystick_x = atlas::event::get_joystic_axis(0, GLFW_GAMEPAD_AXIS_RIGHT_X);

        int button_count;
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &button_count);
        sphere_body->angular_velocity.x = left_joystick_x * 10;
        sphere_body->angular_velocity.y = glm::sin(right_joystick_x) * 10;

        if(buttons[0] == GLFW_PRESS) {
            glm::vec3 linear_velocity = { 0.f, 10.0f, 0.f };
            sphere_body->linear_velocity = linear_velocity;
            sphere_body->cumulative_force += 10.f;
        }

    }
}
