#include "Player_creator.hpp"
#include "Game_object.hpp"
#include "Creator.hpp"
#include "Player.hpp"

#include "vec3.hpp"

#include "Body_2d_def.hpp"
#include "AABB_2d.hpp"
#include "Physics_manager.hpp"

#include "Sprite_atlas.hpp"
#include "Sprite_atlas_manager.hpp"
#include "Animator_controller.hpp"
#include "Sprite_atlas_manager.hpp"

#include "runtime_memory_allocator.hpp"
#include <stdint.h>

Player_creator::Player_creator(const string_id atlas_id, const string_id anim_controller_id) :
	gom::Creator(sizeof(Player)), m_atlas_res_id(atlas_id), m_anim_controller_id(anim_controller_id)
{
	//set the body_def to be able to create body_2d objects for the game object
	void  *pmem = mem::allocate(sizeof(physics_2d::Body_2d_def));
	m_pbody_def = static_cast<physics_2d::Body_2d_def*>( new (pmem) physics_2d::Body_2d_def());

	
	math::vec2 pos(10.0f, 12.0f);

	physics_2d::AABB_2d p_aabb(math::vec2(-0.40f, -0.75f), math::vec2(0.40f, 0.75f));
	p_aabb.p_max += pos;
	p_aabb.p_min += pos;

	m_pbody_def->m_type = physics_2d::Body_2d::Entity_types::DYNAMIC;
	m_pbody_def->m_mass = 1.0f;
	m_pbody_def->m_gravity_scale = 1.0f;
	m_pbody_def->m_position = pos;
	m_pbody_def->m_velocity = math::vec2(0.0f, 0.0f);
	m_pbody_def->m_vel_threshold = math::vec2(6.0f, 12.0f);
	m_pbody_def->m_aabb = p_aabb;

	create_anim_controller();
}

gom::Game_object *Player_creator::create(void *pmem, const uint32_t unique_id, const uint16_t handle_index, const math::vec3 & wld_pos)
{
	//get the sprite_atlas resource 
	gfx::Sprite_atlas *patlas = static_cast<gfx::Sprite_atlas*>(gfx::g_sprite_atlas_mgr.get_by_id(m_atlas_res_id));
	gom::Actor::atlas_n_layer sprite_data(patlas, 1);
	
	//call the player's constructor
	return static_cast<gom::Game_object*>(new (pmem) Player(unique_id, handle_index, sprite_data, m_pbody_def, m_panim_controller));
}

Player_creator::~Player_creator() 
{
	mem::free(static_cast<void*>(m_panim_controller), sizeof(gfx::Animator_controller));
}

void Player_creator::create_anim_controller() 
{
	void *pmem = mem::allocate(sizeof(gfx::Animator_controller));
	m_panim_controller = static_cast<gfx::Animator_controller*>( new (pmem) gfx::Animator_controller() );
	
	//create the animation players for each animation state
	gfx::Animation_player player_idle_anim(gfx::Animation({ 5 }, 5));
	gfx::Animation_player player_running_anim(gfx::Animation({ 8, 10, 9 }, 10));
	gfx::Animation_player player_jumping_anim(gfx::Animation({ 6 }, 1));
	gfx::Animation_player player_climbing_anim(gfx::Animation({ 1, 2 }, 6)); // gfx::Animation({3 }, 6) );
	gfx::Animation_player player_finishing_climbing_anim(gfx::Animation({ 3 }, 6));
	gfx::Animation_player player_attacking_anim(gfx::Animation({ 13, 14 }, 10, false));
	gfx::Animation_player player_ducking_idle_anim(gfx::Animation({ 4 }, 1));
	gfx::Animation_player player_ducking_attacking_anim(gfx::Animation({ 11, 12 }, 10, false));

	//add the parameters to the controller
	m_panim_controller->add_parameter("is_running", gfx::Animator_controller_parameter::Type::BOOL);;
	m_panim_controller->add_parameter("is_jumping", gfx::Animator_controller_parameter::Type::BOOL);
	m_panim_controller->add_parameter("is_climbing", gfx::Animator_controller_parameter::Type::BOOL);
	m_panim_controller->add_parameter("is_ducking", gfx::Animator_controller_parameter::Type::BOOL);
	m_panim_controller->add_parameter("is_attacking", gfx::Animator_controller_parameter::Type::TRIGGER);
	m_panim_controller->add_parameter("finish_climbing", gfx::Animator_controller_parameter::Type::BOOL);

	
	gfx::Animator_state & player_idle_state = m_panim_controller->add_state("player_idle", player_idle_anim);
	gfx::Animator_state & player_running_state = m_panim_controller->add_state("player_running", player_running_anim);
	gfx::Animator_state & player_jumping_state = m_panim_controller->add_state("player_jumping", player_jumping_anim);
	gfx::Animator_state & player_climbing_state = m_panim_controller->add_state("player_climbing", player_climbing_anim);
	gfx::Animator_state & player_finishing_climbing_state = m_panim_controller->add_state("player_finishing_climbing", player_finishing_climbing_anim);
	gfx::Animator_state & player_attacking_state = m_panim_controller->add_state("player_attacking", player_attacking_anim);
	gfx::Animator_state & player_ducking_idle_state = m_panim_controller->add_state("player_ducking_idle", player_ducking_idle_anim);
	gfx::Animator_state & player_ducking_attacking_state = m_panim_controller->add_state("player_ducking_attacking", player_ducking_attacking_anim);

	//transitions from player idle
	gfx::Animator_state_transition & idle_to_ducking_idle = player_idle_state.add_transition("player_ducking_idle");
	idle_to_ducking_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_ducking");

	gfx::Animator_state_transition & idle_to_attack = player_idle_state.add_transition("player_attacking");
	idle_to_attack.add_condition(gfx::Animator_condition::Mode::IF, 1, "is_attacking");

	gfx::Animator_state_transition & idle_to_running = player_idle_state.add_transition("player_running");
	idle_to_running.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_running");

	gfx::Animator_state_transition & idle_to_climbing = player_idle_state.add_transition("player_climbing");
	idle_to_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_climbing");

	gfx::Animator_state_transition & idle_to_jumping = player_idle_state.add_transition("player_jumping");
	idle_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_jumping");

	gfx::Animator_state_transition & running_to_attack = player_running_state.add_transition("player_attacking");
	running_to_attack.add_condition(gfx::Animator_condition::Mode::IF, 1, "is_attacking");

	gfx::Animator_state_transition & running_to_idle = player_running_state.add_transition("player_idle");
	running_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_running");
	running_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");

	gfx::Animator_state_transition & running_to_jumping = player_running_state.add_transition("player_jumping");
	running_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_jumping");
	running_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_running");

	gfx::Animator_state_transition & jumping_to_attacking = player_jumping_state.add_transition("player_attacking");
	jumping_to_attacking.add_condition(gfx::Animator_condition::Mode::IF, 1, "is_attacking");

	gfx::Animator_state_transition & jumping_to_idle = player_jumping_state.add_transition("player_idle");
	jumping_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");
	jumping_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_running");
	jumping_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_climbing");

	gfx::Animator_state_transition & jumping_to_running = player_jumping_state.add_transition("player_running");
	jumping_to_running.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");
	jumping_to_running.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_running");

	gfx::Animator_state_transition & jumping_to_climbing = player_jumping_state.add_transition("player_climbing");
	jumping_to_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_climbing");
	jumping_to_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");

	gfx::Animator_state_transition & climbing_to_finish_climbing = player_climbing_state.add_transition("player_finishing_climbing");
	climbing_to_finish_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "finish_climbing");


	gfx::Animator_state_transition & climbing_to_idle = player_climbing_state.add_transition("player_idle");
	climbing_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_climbing");
	climbing_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");

	gfx::Animator_state_transition & climbing_to_jumping = player_climbing_state.add_transition("player_jumping");
	climbing_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_climbing");
	climbing_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_jumping");

	gfx::Animator_state_transition & finish_climbing_to_idle = player_finishing_climbing_state.add_transition("player_idle");
	finish_climbing_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_climbing");
	finish_climbing_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "finish_climbing");

	gfx::Animator_state_transition & finish_climbing_to_climbing = player_finishing_climbing_state.add_transition("player_climbing");
	finish_climbing_to_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_climbing");
	finish_climbing_to_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "finish_climbing");

	gfx::Animator_state_transition & attacking_to_idle = player_attacking_state.add_transition("player_idle");
	attacking_to_idle.add_condition(gfx::Animator_condition::Mode::IF_NOT, 0, "is_attacking");
	attacking_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");

	gfx::Animator_state_transition & attacking_to_jumping = player_attacking_state.add_transition("player_jumping");
	attacking_to_jumping.add_condition(gfx::Animator_condition::Mode::IF_NOT, 0, "is_attacking");
	attacking_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_jumping");

	gfx::Animator_state_transition & ducking_idle_to_idle = player_ducking_idle_state.add_transition("player_idle");
	ducking_idle_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_ducking");

	gfx::Animator_state_transition & ducking_idle_to_ducking_attacking = player_ducking_idle_state.add_transition("player_ducking_attacking");
	ducking_idle_to_ducking_attacking.add_condition(gfx::Animator_condition::Mode::IF, 1, "is_attacking");

	gfx::Animator_state_transition & ducking_attacking_to_ducking_idle = player_ducking_attacking_state.add_transition("player_ducking_idle");
	ducking_attacking_to_ducking_idle.add_condition(gfx::Animator_condition::Mode::IF_NOT, 0, "is_attacking");

}