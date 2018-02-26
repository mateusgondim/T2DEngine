#ifndef _SPRITE_BATCH_HPP

#define _SPRITE_BATCH_HPP
#include <vector>
#include "Vertex1P1C1UV.hpp"
#include "Texture_2d.hpp"

/*Sprite_batch: part of the tile graphics system, this class is responsable for
 * creating a vao for rendering a group of sprites.
 * Every batch has a texture associated with it and a buffer object to store the sprite's 
 * vertex data
 */
//Forward declaration to prevent includes
namespace gfx { class Shader; class Sprite; }

namespace gfx {
	class Sprite_batch {
	public:
				Sprite_batch(const unsigned max_num_vertices, const Texture_2d & texture, const gfx::Shader & shader, const bool is_static = false);
		void	add(const std::vector<Vertex1P1C1UV> & vertices);
		void    add(const Sprite & sprite);
		void    set_sptexture(const Texture_2d & texture) { m_texture = texture; }
		void	render();
	private:
		bool		m_is_static;
		unsigned	m_max_num_vertices;
		unsigned	m_num_used_vertices;
		Texture_2d  m_texture; // pointer to texture object for the sprites in this batch
		unsigned	m_vao;
		unsigned	m_vbo;
	};
}

#endif // !_SPRITE_BATCH_HPP
