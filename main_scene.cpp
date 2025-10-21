#include "main_scene.hpp"
#include <core/common.hpp>
#include <core/math/utilities.hpp>
#include <core/application.hpp>
#include <core/event/event.hpp>
#include <drivers/jolt-cpp/jolt_components.hpp>
#include <any>

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

    //! @note Creating our objects from our scene
    m_viking_room = create_object("Viking Room Object");
    m_viking_room->set<atlas::transform>({
        .position = {7.20f, 2.70f, -1.70f},
        .rotation = {0.10f, 1.55f, 7.96f},
        .scale{6.f},
    });
    m_viking_room->set<atlas::material>({
        .color = {1.f, 1.f, 1.f, 1.f},
        .model_path = "assets/models/viking_room.obj",
        .texture_path = "assets/models/viking_room.png"
    });

    m_viking_room->add<atlas::tag::serialize>();

    m_viking_room->set<atlas::box_collider>({
        .half_extent = {6.f, 6.f, 6.f}
    });
    m_viking_room->set<atlas::physics_body>({
        // .body_type = atlas::dynamic
        .body_movement_type = atlas::fixed
    });

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
    m_viking_room->set<atlas::transform>({
        .position = {7.20f, 31.00, -1.70f},
        .rotation = {0.10f, 1.55f, 7.96f},
        .scale{6.f},
    });

    m_sphere->set<atlas::transform>({
        .position = {-2.70f, 2.70, -8.30f},
        .scale{1.f}
    });
    glm::vec3 sphere_scale{1.f};
    m_sphere->set<atlas::transform>({
        .position = {-2.70f, 2.70, -8.30f},
        .rotation = {2.30f, 95.90f, 91.80f},
        .scale = sphere_scale,
    });


}


void edit_transform(atlas::transform* p_transform) {
    atlas::ui::draw_vec3("Position", p_transform->position);
    atlas::ui::draw_vec3("Scale", p_transform->scale);
    atlas::ui::draw_vec3("Rotation", p_transform->rotation);
}

void main_scene::collision_enter(atlas::event::collision_enter& p_event) {
    // console_log_warn("Collision Enter happened!!! Executed from main_scene::collision_enter");

    flecs::world registry = *this;

    flecs::entity e1 = registry.entity(p_event.entity1);
    flecs::entity e2 = registry.entity(p_event.entity2);

}

void main_scene::collision_persisted(atlas::event::collision_persisted& p_event) {
    flecs::world registry = *this;

    flecs::entity e1 = registry.entity(p_event.entity2);


    // if(e1.name() == "Sphere") {
    //     console_log_info("Sphere Was Noticed!!!");
    // }
}

void main_scene::start_game() {
    // Was only used for testing purpose. Can remove if you want
    
    m_deserializer_test = atlas::serializer();
    if(!m_deserializer_test.load("LevelScene", *this)) {
        console_log_error("Cannot load LevelScene!!!");
    }
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

    bool controller = atlas::event::is_joystic_present(0);

    // console_log_info("Joystick 0 = {}", atlas::event::is_joystick_button_pressed(GLFW_JOYSTICK_1));
    // console_log_info("Joystick 1 = {}", atlas::event::is_joystick_button_pressed(GLFW_JOYSTICK_2));
    // console_log_info("Joystick 2 = {}", atlas::event::is_joystick_button_pressed(GLFW_JOYSTICK_3));
    // console_log_info("Joystick 3 = {}", atlas::event::is_joystick_button_pressed(GLFW_JOYSTICK_4));

    // console_log_info("Controller 0 = {}", controller);

    // auto button = atlas::event::joystick_button(0);
    // float first_joystick = atlas::event::get_joystic_axis(0, 0);
    // console_log_info("Axis 1 = {}", first_joystick); 

    // console_log_info("Button Name = {}", button.Name);
    // console_log_info("Button ID = {}", button.ID);
}

void
main_scene::on_physics_update() {
    float dt = atlas::application::delta_time();
    atlas::physics_body* sphere_body = m_sphere->get_mut<atlas::physics_body>();

    if (atlas::event::is_key_pressed(key_r) and !m_physics_is_runtime) {
        runtime_start();
    }

    if(m_physics_is_runtime) {
        m_physics_engine_handler.update(dt);
    }

    if (atlas::event::is_key_pressed(key_l) and m_physics_is_runtime) {
        runtime_stop();
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
    // console_log_info("Bool = {}", controller_connected);

    if(controller_connected) {
        float speed = 10.f;
        float left_joystick_x = atlas::event::get_joystic_axis(0, GLFW_GAMEPAD_AXIS_LEFT_X);
        float left_joystick_y = atlas::event::get_joystic_axis(0, GLFW_GAMEPAD_AXIS_LEFT_Y);
        float right_joystick_x = atlas::event::get_joystic_axis(0, GLFW_GAMEPAD_AXIS_RIGHT_X);

        int button_count;
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &button_count);
        sphere_body->angular_velocity.x = left_joystick_x * 10;
        // auto test = atlas::event::joystick_button(0);
        // console_log_info("second_joystick = {}", second_joystick);
        // if (left_joystick_x > 0.1f) {
        //     // printf("⬆️ Moving Up (Negative)\n");
        //     // glm::vec3 position = {10.f, 0.f, 0.f};
        //     sphere_body->angular_velocity.x = 10.f;
        //     // console_log_info("MOving Up (negative!)");
        // }
        // if(left_joystick_y < -0.1f) {
        //     sphere_body->angular_velocity.x = -10.f;
        // }

        if(buttons[0] == GLFW_PRESS) {
            console_log_info("buttons[0] Pressed!");
            glm::vec3 linear_velocity = { 0.f, sphere_body->angular_velocity.x + 10.0f, 0.f };
            sphere_body->linear_velocity = linear_velocity;
        }
        else if(buttons[1] == GLFW_PRESS) {
            // console_log_info("buttons[1] Pressed!");
            // glm::vec3 linear_velocity = { 0.f, 10.0f, 0.f };
            // sphere_body->linear_velocity = linear_velocity;
        }

        auto button_a = atlas::event::joystick_button(0);
        bool button_pressed = atlas::event::is_joystick_button_pressed(button_a.ID);
        if(button_pressed) {
            console_log_info("button_a pressed!");
        }
        // if(button_a.ButtonState == atlas::event::input_state::Pressed) {
        //     console_log_info("button_a pressed!");
        // }

        console_log_info("left_x = {}", left_joystick_x);
        console_log_info("left_y = {}", left_joystick_y);

        // if (left_joystick_y < -0.1f or left_joystick_y > 0.1f) {
        //     // printf("⬆️ Moving Up (Negative)\n");
        //     sphere_body->angular_velocity.y = left_joystick_y * 10;
        //     // console_log_info("MOving Up (negative!)");
        // }

    }
}
