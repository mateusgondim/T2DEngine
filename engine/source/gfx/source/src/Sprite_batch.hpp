#ifndef _SPRITE_BATCH_HPP

#define _SPRITE_BATCH_HPP

#define VERTICES_PER_SPRITE 6
#include <vector>
#include <cstdint>
#include "Vertex1P1C1UV.hpp"

/*Sprite_batch: part of the graphics system, this class is responsable for
 * creating a vao for rendering a group of sprites.
 * The group of sprites are all on the same layer and uses the same texture.
 * The layer of the batch is managed by the graphics manager and, the id of the texture
 * is kept on the batch's data member 'm_texture_id', witch is the conversion of the texture
 * file name to a unsigned of 32 bits using the crc32 algorithm
 */
namespace gfx { class Sprite; struct Vertex1P1C1UV; }

/* TODO: Refactor
 * - Remove m_sprite_aux_buffer data member
 * - delete all the copy controll member functions
 */

namespace gfx {
	class Sprite_batch {
	public:
				Sprite_batch(const unsigned max_num_vertices, const bool is_static = false);
		void	add(const std::vector<Vertex1P1C1UV> & vertices);
        void    add(const Vertex1P1C1UV * pvertex_buffer, const std::size_t buffer_sz);
		void    add(const Sprite *psprite);
	
		void	render();
        void    reset();

		~Sprite_batch(); // deallocate opengl Resources
	private:
		std::uint32_t	m_max_num_vertices;
		std::uint32_t	m_num_used_vertices;
        Vertex1P1C1UV   m_sprite_aux_buffer[VERTICES_PER_SPRITE];
		std::uint32_t	m_vao;
		std::uint32_t	m_vbo;
		bool			m_is_static;
	};
}

#endif // !_SPRITE_BATCH_HPP
