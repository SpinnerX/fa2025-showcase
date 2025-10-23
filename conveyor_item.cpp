#include "conveyor_item.hpp"
#include <core/common.hpp>
#include <core/scene/components.hpp>
#include <core/math/utilities.hpp>

conveyor_item::conveyor_item(flecs::entity& p_entity) : m_entity(&p_entity) {
    // set direction to the forward direction
    m_direction = glm::rotate(atlas::to_quat(m_entity->get<atlas::transform>()->quaternion), glm::vec3(0.f, 0.f, -1.f));
}