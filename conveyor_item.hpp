#pragma once
#include <flecs.h>
#include <glm/glm.hpp>

class conveyor_item {
public:
    conveyor_item() = default;
    conveyor_item(flecs::entity& p_entity);

private:
    flecs::entity* m_entity;
    glm::vec3 m_direction;
    glm::vec3 m_respawn_startpoint;
    glm::vec3 m_respawn_endpoint;


};