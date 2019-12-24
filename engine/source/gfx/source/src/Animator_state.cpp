#include "Animator_state.hpp"
#include "string_id.hpp"
#include "Animation_player.hpp"

gfx::Animator_state::Animator_state(string_id name_id) : m_name_id(name_id) {}

gfx::Animator_state::Animator_state(string_id name_id, const gfx::Animation_player & anim_player) : 
        m_name_id(name_id) , m_anim_player(anim_player) {}

gfx::Animator_state_transition & gfx::Animator_state::add_transition(const char *destination_state_name) 
{
	Animator_state_transition transition(destination_state_name);
	m_transitions.push_back(transition);

	return m_transitions.back();
}

gfx::Animator_state_transition & gfx::Animator_state::add_transition(string_id destination_state_id) 
{
	Animator_state_transition transition(destination_state_id);
	m_transitions.push_back(transition);

	return m_transitions.back();
}

/* There is a limitation on the Animation system, since the only way to identify a transition
 * is by the transition's destination state, we cant have two diferent transitions tha go to 
 * the same destination state!! they will be considered by the system as the same transition.
 * We need to add an ID to each transition, so we can have more than transition
 * to the same destination state
 */
void gfx::Animator_state::remove_transition(const gfx::Animator_state_transition & transition) 
{
	std::vector<Animator_state_transition>::iterator it;
	for (it = m_transitions.begin(); it != m_transitions.end(); ++it) {
		if (it->get_destination_state_id() == transition.get_destination_state_id()) { // CHANGE TO IT->ID == transition.get_id()
			m_transitions.erase(it);
			break;
		}
	}
}

void gfx::Animator_state::update(const float dt) 
{
	m_anim_player.update(dt);
}

void gfx::Animator_state::pause_anim() 
{
	m_anim_player.pause();
}

void gfx::Animator_state::resume_anim() 
{
	m_anim_player.resume();
}

void gfx::Animator_state::start_anim() 
{
	m_anim_player.start_from_beg();
}

bool gfx::Animator_state::is_anim_playing() const 
{
	return m_anim_player.is_playing();
}

std::ostream & gfx::operator<<(std::ostream & os, const Animator_state & state) 
{
	os << "State name = " << get_string_by_id(state.m_name_id) << std::endl;
	os << "Transitions from state: " << std::endl;
	for (std::vector<gfx::Animator_state_transition>::const_iterator it = state.m_transitions.cbegin(); it != state.m_transitions.cend(); ++it) {
		os << '\t' << *it;
		os << std::endl;
	}
	
	return os;
}