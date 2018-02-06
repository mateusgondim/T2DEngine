#ifndef _ENGINE_HPP
#define _ENGINE_HPP

//cgm
#include "vec3.hpp"
#include "mat4.hpp"
#include "matrix_transform.hpp"
#include "Transform.hpp"

//utility
#include "Timer.hpp"
#include "Rect.hpp"

//graphics
#include "Camera_2d.hpp"
#include "Tile_map.hpp"
#include "Tile_map_renderer.hpp"
#include "Shader.hpp"
#include "tmx_parser.hpp"
#include "Sprite_batch.hpp"

#include "Animation.hpp"
#include "Animation_player.hpp"
#include "Animation_state.hpp"
#include "Transition.hpp"
#include "Animator_controller.hpp"

//physics
#include "Body_2d.hpp"
#include "Collision_listener.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"

//input
#include "Button.hpp"
#include "Input_manager.hpp"

//game object model
#include "Game_object.hpp"
#include "Ren_coll_game_object.hpp"
#include "Gameplay_state.hpp"
#include "Actor.hpp"


class Engine final {
public:
	Engine() = default;
	void init();
	//void shut_down();
	~Engine() {}
public:
	Timer m_timer;
	physics_2d::Physics_manager  m_physics_manager;
	//graphics_manager m_graphics_manager
	Input_manager    m_input_manager;
};

extern Engine g_engine;
#endif // !_ENGINE_HPP
