#include "editor_panels.hpp"
#include <imgui.h>
#include <core/scene/components.hpp>
#include <physics/components.hpp>
#include <core/ui/widgets.hpp>

editor_panel::editor_panel(flecs::world& p_registry, atlas::event::event_bus& p_bus) : m_registry(&p_registry), m_bus(&p_bus) {
}

void editor_panel::defer_begin() {
    m_registry->defer_begin();
}

void editor_panel::defer_end() {
    m_registry->defer_end();
}

void
ui_component_list(flecs::entity& p_selected_entity) {
    std::string entity_name = p_selected_entity.name().c_str();

    atlas::ui::draw_input_text(entity_name);
    p_selected_entity.set_name(entity_name.c_str());

    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("Add Component");
    }

    if (ImGui::BeginPopup("Add Component")) {
        if (!p_selected_entity.has<atlas::perspective_camera>()) {
            if (ImGui::MenuItem("Perspective Camera")) {
                p_selected_entity.add<
                  flecs::pair<atlas::tag::editor, atlas::projection_view>>();
                p_selected_entity.add<atlas::perspective_camera>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!p_selected_entity.has<atlas::material>()) {
            if (ImGui::MenuItem("Material (Mesh Component)")) {
                p_selected_entity.add<atlas::material>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!p_selected_entity.has<atlas::tag::serialize>()) {
            if (ImGui::MenuItem("Serialize")) {
                p_selected_entity.add<atlas::tag::serialize>();
                ImGui::CloseCurrentPopup();
            }
        }

        // NOTE -- Add this in later...
        // if (!p_selected_entity.has<atlas::tag::serialize>()) {
        //     if (ImGui::MenuItem("Tag::Serialize")) {
        //         p_selected_entity.add<atlas::tag::serialize>();
        //         ImGui::CloseCurrentPopup();
        //     }
        // }

        if (!p_selected_entity.has<atlas::physics_body>()) {
            if (ImGui::MenuItem("Physics Body")) {
                p_selected_entity.add<atlas::physics_body>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!p_selected_entity.has<atlas::box_collider>()) {
            if (ImGui::MenuItem("Box Collider")) {
                p_selected_entity.add<atlas::box_collider>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!p_selected_entity.has<atlas::sphere_collider>()) {
            if (ImGui::MenuItem("Sphere Collider")) {
                p_selected_entity.add<atlas::sphere_collider>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!p_selected_entity.has<atlas::capsule_collider>()) {
            if (ImGui::MenuItem("Capsule Collider")) {
                p_selected_entity.add<atlas::capsule_collider>();
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::PopItemWidth();
}


void editor_panel::render_properties_panel() {
    defer_begin();
    auto query_builder = m_registry->query_builder<atlas::transform>().build();

    if (ImGui::Begin("Scene Heirarchy")) {

        query_builder.each([&](flecs::entity p_entity, atlas::transform&) {
            // We set the imgui flags for our scene heirarchy panel
            // TODO -- Make the scene heirarchy panel a separate class that is
            // used for specify the layout and other UI elements here
            ImGuiTreeNodeFlags flags =
              ((m_selected_entity == p_entity) ? ImGuiTreeNodeFlags_Selected
                                               : 0) |
              ImGuiTreeNodeFlags_OpenOnArrow;
            flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
            flags |= ImGuiWindowFlags_Popup;
            bool opened = ImGui::TreeNodeEx(p_entity.name().c_str(), flags);
            if (ImGui::IsItemClicked()) {
                m_selected_entity = p_entity;
                // m_create_entity = search_entity(p_entity.name().c_str());
            }

            bool delete_entity = false;
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Delete Entity")) {
                    delete_entity = true;
                }
                ImGui::EndPopup();
            }

            if (delete_entity) {
                // _context->destroyEntity(entity);
                m_selected_entity.destruct();
            }

            if (opened) {
                flags = ImGuiTreeNodeFlags_OpenOnArrow |
                        ImGuiTreeNodeFlags_SpanAvailWidth;
                auto query_children_builder =
                  m_registry->query_builder().with(flecs::ChildOf, p_entity).build();
                int32_t child_count = query_children_builder.count();

                // // Only show children in scene heirarchy panel if there are
                // children entities
                if (child_count > 0) {
                    m_selected_entity.children(
                      [&](flecs::entity p_child_entity) {
                          opened = ImGui::TreeNodeEx(
                            p_child_entity.name().c_str(), flags);
                          if (opened) {
                              if (ImGui::IsItemClicked()) {
                                  m_selected_entity = p_child_entity;
                                  // m_create_entity =
                                  // search_entity(p_child_entity.name().c_str());
                              }
                              ImGui::TreePop();
                          }
                      });
                }

                ImGui::TreePop();
            }
        });

        defer_end();
        ImGui::End();
    }

    if (ImGui::Begin("Properties")) {
        if (m_selected_entity.is_alive()) {
            ui_component_list(m_selected_entity);

            atlas::ui::draw_component<atlas::transform>(
              "transform",
              m_selected_entity,
              [](atlas::transform* p_transform) {
                  atlas::ui::draw_vec3("Position", p_transform->position);
                  atlas::ui::draw_vec3("Scale", p_transform->scale);
                  atlas::ui::draw_vec3("Rotation", p_transform->rotation);
              });

            atlas::ui::draw_component<atlas::perspective_camera>(
              "camera",
              m_selected_entity,
              [](atlas::perspective_camera* p_camera) {
                  atlas::ui::draw_float("field of view",
                                        p_camera->field_of_view);
                  ImGui::Checkbox("is_active", &p_camera->is_active);
              });

            atlas::ui::draw_component<atlas::material>(
              "material", m_selected_entity, [](atlas::material* p_material) {
                  atlas::ui::draw_input_text(p_material->model_path);
                  atlas::ui::draw_vec4("Color", p_material->color);
              });

            atlas::ui::draw_component<atlas::physics_body>(
              "Physics Body",
              m_selected_entity,
              [](atlas::physics_body* p_body) {
                  const char* items[] = {
                      "Static",
                      "Kinematic",
                      "Dynamic",
                  };
                  const char* combo_preview = items[p_body->body_movement_type];

                  // Begin the combo box
                  if (ImGui::BeginCombo("Body Type", combo_preview)) {
                      for (int n = 0; n < 3; n++) {
                          // Check if the current item is selected
                          const bool is_selected =
                            (p_body->body_movement_type == n);
                          if (ImGui::Selectable(items[n], is_selected)) {
                              // Update the current type when a new item is
                              // selected
                              p_body->body_movement_type =
                                static_cast<atlas::body_type>(n);
                          }

                          // Set the initial focus when the combo box is first
                          // opened
                          if (is_selected) {
                              ImGui::SetItemDefaultFocus();
                          }
                      }
                      ImGui::EndCombo();
                  }

                  // physics body parameters
                  atlas::ui::draw_vec3("Linear Velocity",
                                       p_body->linear_velocity);
                  atlas::ui::draw_vec3("Angular Velocity",
                                       p_body->angular_velocity);
                  atlas::ui::draw_vec3("Force", p_body->cumulative_force);
                  atlas::ui::draw_float("Friction", p_body->friction);
                  atlas::ui::draw_vec3("Torque", p_body->cumulative_torque);
                  atlas::ui::draw_vec3("Center Mass",
                                       p_body->center_mass_position);
              });

            atlas::ui::draw_component<atlas::box_collider>(
              "Box Collider",
              m_selected_entity,
              [](atlas::box_collider* p_collider) {
                  atlas::ui::draw_vec3("Half Extent", p_collider->half_extent);
              });

            atlas::ui::draw_component<atlas::sphere_collider>(
              "Box Collider",
              m_selected_entity,
              [](atlas::sphere_collider* p_collider) {
                  atlas::ui::draw_float("Radius", p_collider->radius);
              });

            atlas::ui::draw_component<atlas::capsule_collider>(
              "Box Collider",
              m_selected_entity,
              [](atlas::capsule_collider* p_collider) {
                  atlas::ui::draw_float("Half Height", p_collider->half_height);
                  atlas::ui::draw_float("Radius", p_collider->radius);
              });

            atlas::ui::draw_component<atlas::tag::serialize>(
              "Serialize",
              m_selected_entity,
              [](atlas::tag::serialize* p_serialize) {
                  ImGui::Checkbox("Enable", &p_serialize->enable);
              });
        }

        ImGui::End();
    }
}