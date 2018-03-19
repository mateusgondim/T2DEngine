#ifndef _SPRITE_BATCH_HPP

#define _SPRITE_BATCH_HPP
#include <vector>
#include <cstdint>

/*Sprite_batch: part of the graphics system, this class is responsable for
 * creating a vao for rendering a group of sprites.
 * The group of sprites are all on the same layer and uses the same texture.
 * The layer of the batch is managed by the graphics manager and, the id of the texture
 * is kept on the batch's data member 'm_texture_id', witch is the conversion of the texture
 * file name to a unsigned of 32 bits using the crc32 algorithm
 */
//Forward declaration to prevent includes
namespace gfx { class Sprite; struct Vertex1P1C1UV; }

namespace gfx {
	class Sprite_batch {
	public:
				Sprite_batch(const unsigned max_num_vertices, const bool is_static = false);
		void	add(const std::vector<Vertex1P1C1UV> & vertices);
		void    add(const Sprite *psprite);
	
		void	render();

		~Sprite_batch(); // deallocate opengl Resources
	private:
		std::uint32_t	m_max_num_vertices;
		std::uint32_t	m_num_used_vertices;
		std::uint32_t	m_vao;
		std::uint32_t	m_vbo;
		bool			m_is_static;
	};
}

#endif // !_SPRITE_BATCH_HPP
