#ifndef _TILE_MAP_RENDERER_HPP
#define _TILE_MAP_RENDERER_HPP

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include "vec2.hpp"
#include "vec3.hpp"
#include "Tile_map.hpp"
#include "Shader.hpp"

#define TILE_MAP_DEBUG_OPTIONS 2


#define glCheckError() glCheckError_(__FILE__, __LINE__) 

class Tile_map_renderer {
public:
	enum Debug_options {DISPLAY_GRID = 0, DISPLAY_COLLIDERS = 1};
	Tile_map_renderer(const Tile_map & tile_map,const gls::Shader & shader);
	void render(const gls::Shader & shader);
	void set_debug_mode(Debug_options option, const gls::Shader & shader);
	void switch_debug_option(Debug_options option, const bool value);
private:
	void setup_buffers(const gls::Shader & shader);
	const Tile_map *m_tmap_ptr;
	
	std::vector<cgm::vec3> m_vertices_pos;
	std::vector<cgm::vec2> m_vextices_text_coord;
	std::vector<GLuint>	   m_indices;
	GLuint m_VAO;
	GLuint m_VBO_pos;
	GLuint m_VBO_uv;
	GLuint m_EBO;

	/* Debug: Define NTILE_MAP_DEBUG to turn off all map debug information
	 *
	 */
#ifndef NTILE_MAP_DEBUG
	bool    m_debug_flags[TILE_MAP_DEBUG_OPTIONS] = {false};
	GLuint  m_debug_VAOS[TILE_MAP_DEBUG_OPTIONS];
	GLuint  m_debug_VBOS[TILE_MAP_DEBUG_OPTIONS];
	int     m_debug_num_colliders;
	const  gls::Shader *m_debug_pshaders[TILE_MAP_DEBUG_OPTIONS];
	
#endif // !NTILE_MAP_DEBUG

};

GLenum glCheckError_(const char *file, int line);
#endif // !_TILE_MAP_RENDERER_HPP
