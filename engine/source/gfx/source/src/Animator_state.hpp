#ifndef _ANIMATOR_STATE_HPP
#define _ANIMATOR_STATE_HPP	

#include "string_id.hpp"
#include "Animation_player.hpp"
#include "Animator_state_transition.hpp"

#include <iostream>

namespace gfx {
	class Animator_state final {
		friend std::ostream & operator<<(std::ostream & os, const Animator_state & state);
	public:
		Animator_state() = default;
		Animator_state(string_id name_id);
		Animator_state(string_id name_id, const Animation_player & anim_player);

		Animator_state_transition & add_transition(string_id destination_state_id);
		Animator_state_transition & add_transition(const char *destination_state_name);

		void	remove_transition(const Animator_state_transition & transition);

		std::vector<Animator_state_transition> & get_transitions() { return m_transitions; }
		string_id   get_state_id() const { return m_name_id; }
		Animation_player & get_animation_player() { return m_anim_player; }

		bool changed_animation_frame() const { return m_anim_player.changed_frame(); }
		string_id get_curr_anim_frame() const { return m_anim_player.get_frame(); }
		
		void update(const float dt);
		void pause_anim();
		void resume_anim();
		void start_anim();
		bool is_anim_playing() const;
	private:
		string_id m_name_id;
		std::vector<Animator_state_transition> m_transitions;
		Animation_player					   m_anim_player;
	};

	std::ostream & operator<<(std::ostream & os, const Animator_state & state);
}

#endif // !_ANIMATOR_STATE_HPP
