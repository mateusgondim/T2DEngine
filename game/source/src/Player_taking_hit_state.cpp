#include "Player_taking_hit_state.hpp"
#include "Actor.hpp"
#include "AABB_2d.hpp"

#include <algorithm>

#include "Player.hpp"
#include "Player_idle_state.hpp"
#include "Player_climbing_state.hpp"

#include "Game_object_handle.hpp"
#include "Projectile_manager.hpp"

#include "Animator_controller.hpp"
#include "Gameplay_state.hpp"
#include "vec2.hpp"

#include "Body_2d.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"

#include "runtime_memory_allocator.hpp"
#include "crc32.hpp"

Player_taking_hit_state::Player_taking_hit_state(gom::Actor & actor, float jump_vel, float x_vel) : Gameplay_state()
{
        actor.get_body_2d_component()->stop_movement_x();
        actor.get_body_2d_component()->stop_movement_y();
        x_vel = (actor.get_facing_direction()) ? (x_vel) : (-x_vel);
        actor.get_body_2d_component()->add_to_velocity(math::vec2(x_vel, jump_vel));
}

gom::Gameplay_state * Player_taking_hit_state::handle_input(gom::Actor & actor) 
{
    physics_2d::World * pworld = physics_2d::g_physics_mgr.get_world();
    bool on_ground = pworld->is_body_2d_on_ground(actor.get_body_2d_component());
    if (on_ground) {
        actor.get_body_2d_component()->stop_movement_x();
        Player *pplayer = static_cast<Player*>(&actor);
        pplayer->set_taking_hit(false);
        bool is_player_dead = pplayer->is_player_dead();
        if (!is_player_dead) {
            // Set paramter on animation state machine to make the transition to the new animation
            actor.get_anim_controller_component()->set_bool(SID('is_taking_hit'), false);
            // start invincibility mode
            pplayer->start_invincibiliy_mode();

            void *pmem = mem::allocate(sizeof(Player_idle_state));
            return static_cast<gom::Gameplay_state*> (new (pmem) Player_idle_state);
        }
        else {
            return nullptr;
        }

    }
    return nullptr;
}


size_t Player_taking_hit_state::get_size() const
{
        return sizeof(Player_taking_hit_state);
}


void Player_taking_hit_state::begin_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
        std::cout << __FUNCTION__ << std::endl;
}

void Player_taking_hit_state::end_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
        std::cout << __FUNCTION__ << std::endl;
}
