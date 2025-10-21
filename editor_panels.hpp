#pragma once
#include <core/event/event_bus.hpp>
#include <flecs.h>

class editor_panel {
public:
    editor_panel() = default;
    editor_panel(flecs::world& p_registry, atlas::event::event_bus& p_bus);

    void render_properties_panel();

private:
    void defer_begin();
    void defer_end();

private:
    flecs::entity m_selected_entity{flecs::entity::null()};
    flecs::world* m_registry;
    atlas::event::event_bus* m_bus=nullptr;
};