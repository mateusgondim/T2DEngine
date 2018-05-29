#ifndef _CREATOR_HPP
#define _CREATOR_HPP

#include <cstddef>
#include <stdint.h>

/* Creator: this class is used with the Game_object_factory class, to be able to dynamically create objects at runtime 
 * Every class that wants to register itself with the factory, should have a correspoding Creator class, i.e
 * the Zombie class should have a Creator_zombie that inherits from the Creator
 */

namespace gom { class Game_object; }
namespace physics_2d {struct Body_2d_def; }

namespace gom {
	class Creator {
	public:
		Creator(const std::size_t sz) : m_pbody_def(nullptr), m_size(sz) {}
		virtual ~Creator();

		virtual Game_object *create(void * pmem, const uint32_t unique_id, const uint16_t handle_index) = 0;
		std::size_t get_size() const { return m_size; }
	protected:
		physics_2d::Body_2d_def *m_pbody_def; // used to create Body_2d for game objects that need physics simulation
	private:
		std::size_t m_size; // size in bytes
	};
}
#endif // !_CREATOR_HPP
