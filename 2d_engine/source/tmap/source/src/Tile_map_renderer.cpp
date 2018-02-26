#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "Rect.hpp"
#include "Tile_map_renderer.hpp"
#include "Tile_map.hpp"
#include "vec3.hpp"
#include "Shader.hpp"


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cerr << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}


Tile_map_renderer::Tile_map_renderer(const Tile_map & tile_map, const gfx::Shader & shader)
{
	m_tmap_ptr = &tile_map;

#ifndef NTILE_MAP_DEBUG
	for (int i = 0; i != TILE_MAP_DEBUG_OPTIONS; ++i) {
		m_debug_flags[i]     =  false;
		m_debug_VAOS[i]      =  0;
		m_debug_VBOS[i]      =  0;
		m_debug_pshaders[i]  =  nullptr;
	}
	m_debug_num_colliders = 0;
#endif

	setup_buffers(shader);
}

//shader in paramenter list, shader's use() function need to be call at the start of this function!!!!!!!!!!!!!!!
void Tile_map_renderer::setup_buffers(const gfx::Shader & shader)
{

	
	//calculate the tile's world position
	for (int i = 0; i != m_tmap_ptr->height(); ++i) {
		for (int j = 0; j != m_tmap_ptr -> width(); ++j) {
			cgm::vec3 v_pos0, v_pos1, v_pos2, v_pos3;
			Rect rect = m_tmap_ptr->tile_wld_space_bounds(i, j);

			v_pos0.x = rect.x;
			v_pos0.y = rect.y;
			v_pos0.z = 0;

			v_pos1.x = rect.x;
			v_pos1.y = rect.y - rect.height;
			v_pos1.z = 0;

			v_pos2.x = rect.x + rect.width;
			v_pos2.y = rect.y - rect.height;
			v_pos2.z = 0;

			v_pos3.x = rect.x + rect.width;
			v_pos3.y = rect.y;
			v_pos3.z = 0;

			m_vertices_pos.push_back(v_pos0); // top left
			m_vertices_pos.push_back(v_pos1); // bottom left
			m_vertices_pos.push_back(v_pos2); // bottom right
			
			m_vertices_pos.push_back(v_pos2); // bottom right
			m_vertices_pos.push_back(v_pos3); // top right
			m_vertices_pos.push_back(v_pos0); // top left
		}
	}
	/*
	for (int i = 0; i < m_tmap_ptr->height(); ++i) {
		for (int j = 0; j < m_tmap_ptr->width(); ++j) {
			int r = 4 * m_tmap_ptr->width() * i;
			m_indices.push_back(r + (j * 4) ); // top left
			m_indices.push_back(r + (j * 4) + 1); // bottom left
			m_indices.push_back(r + (j * 4) + 2); // bottom right
			
			m_indices.push_back(r + (j * 4) + 2); // bottom right
			m_indices.push_back(r + (j * 4) + 3); // top right
			m_indices.push_back(r + (j * 4)); // top left
		}
	}*/

	/*TODO: For every layer, find the tileset assigned to that layer,
	  than calculate for that layer the uv coordinates of each tile
	*/
	
	for (int i = 0; i < m_tmap_ptr->height(); ++i) {
		for (int j = 0; j < m_tmap_ptr->width(); ++j) {
			unsigned tile_id = m_tmap_ptr->get_tile_id(0, i, j);
			cgm::vec2 uv0, uv1, uv2, uv3;
			
			//find witch tileset has the tile width id
			const Tileset * ptileset = nullptr;
			for (auto & tileset : m_tmap_ptr ->get_tilesets()) {
				if (tileset.is_inside_set(tile_id)) {
					ptileset = &tileset;
					break;
				}
			}
			if (ptileset) {
				ptileset->get_text_coord(tile_id, uv0, uv1, uv2, uv3);
			}
#ifndef NDEBUG
			else {
				std::cerr << __FUNCTION__ << " ERROR: Could not find tileset containing tile with id = " << tile_id << std::endl;
			}
#endif // !NDEBUG

			m_vextices_text_coord.push_back(uv0);
			m_vextices_text_coord.push_back(uv1);
			m_vextices_text_coord.push_back(uv2);
			
			m_vextices_text_coord.push_back(uv2);
			m_vextices_text_coord.push_back(uv3);
			m_vextices_text_coord.push_back(uv0);
		}
	}

	shader.use();
	// Create buffers/arrays
	glGenVertexArrays(1, &m_VAO);
	glCheckError();
	glGenBuffers(1, &m_VBO_pos);
	glCheckError();
	glGenBuffers(1, &m_VBO_uv);
	glCheckError();
	//glGenBuffers(1, &m_EBO);
	//glCheckError();

	glBindVertexArray(m_VAO);
	glCheckError();
	// Load data into vertex buffers
	
	//vertex postion
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_pos);
	glCheckError();
	glBufferData(GL_ARRAY_BUFFER, m_vertices_pos.size() * sizeof(cgm::vec3), &m_vertices_pos[0], GL_STATIC_DRAW);
	glCheckError();

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	//glCheckError();
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);
	//glCheckError();

	glEnableVertexAttribArray(0);
	glCheckError();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cgm::vec3), (GLvoid*)0);
	glCheckError();

	//vertex texture coordinate
 	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_uv);
	glCheckError();
	glBufferData(GL_ARRAY_BUFFER, m_vextices_text_coord.size() * sizeof(cgm::vec2), &m_vextices_text_coord[0], GL_STATIC_DRAW);
	glCheckError();

	glEnableVertexAttribArray(1);
	glCheckError();
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(cgm::vec2), (GLvoid*)0);
	glCheckError();
	glBindVertexArray(0);
	glCheckError();

}

void Tile_map_renderer::set_debug_mode(Debug_options option, const gfx::Shader & shader)
{
	std::vector<cgm::vec3> position;
#ifndef NTILE_MAP_DEBUG //if it is in map debug mode 
	switch (option) {
	case DISPLAY_GRID:
		if (m_debug_flags[DISPLAY_GRID]) { //already set
			break;
		}
		else if (glIsVertexArray(m_debug_VAOS[DISPLAY_GRID]) == GL_TRUE ) { // alredy set
			std::cerr << __FUNCTION__ << " ERROR: Debug option 'DISPLAY_GRID' is already set" << std::endl;
			break;
		}

		
		//SET THE SHADER, SET THE BUFFERS AND SET FLAG TO TRUE
		
		//shader
		m_debug_pshaders[DISPLAY_GRID] = &shader;
		//buffers
		m_debug_pshaders[DISPLAY_GRID]->use();
		for (int j = 0; j != m_tmap_ptr->width(); ++j ) { // for each column in the first row get the vertical lines of the grid
			cgm::vec3 v0, v1;
			
			Rect rect = m_tmap_ptr->tile_wld_space_bounds(0, j);
			v0.x = rect.x;
			v0.y = rect.y;

			rect = m_tmap_ptr->tile_wld_space_bounds(m_tmap_ptr->height() - 1, j);
			v1.x = rect.x;
			v1.y = rect.y - rect.height;
			position.push_back(v0);
			position.push_back(v1);

			if (j == m_tmap_ptr->width() - 1) { // last vertical line
				rect = m_tmap_ptr->tile_wld_space_bounds(0, j);
				v0.x = rect.x + rect.width;
				v0.y = rect.y;

				rect = m_tmap_ptr->tile_wld_space_bounds(m_tmap_ptr->height() - 1, j);
				v1.x = rect.x + rect.width ;
				v1.y = rect.y - rect.height;
				position.push_back(v0);
				position.push_back(v1);
			}
		}

		// horizontal grid lines
		for (int i = 0; i != m_tmap_ptr->height(); ++i) {
			cgm::vec3 v0, v1;

			Rect rect = m_tmap_ptr->tile_wld_space_bounds(i, 0);
			v0.x = rect.x;
			v0.y = rect.y;

			rect = m_tmap_ptr->tile_wld_space_bounds(i, m_tmap_ptr->width() - 1);
			v1.x = rect.x + rect.width;
			v1.y = rect.y;
			position.push_back(v0);
			position.push_back(v1);

			if (i == m_tmap_ptr -> height() - 1) {
				rect = m_tmap_ptr->tile_wld_space_bounds(i, 0);
				v0.x = rect.x;
				v0.y = rect.y - rect.height;

				rect = m_tmap_ptr->tile_wld_space_bounds(i, m_tmap_ptr->width() - 1);
				v1.x = rect.x + rect.width;
				v1.y = rect.y - rect.height;
				position.push_back(v0);
				position.push_back(v1);
			}
		}

		// Create buffers/arrays
		glGenVertexArrays(1, &m_debug_VAOS[DISPLAY_GRID]);
		glCheckError();
		glGenBuffers(1, &m_debug_VBOS[DISPLAY_GRID]);
		glCheckError();

		glBindVertexArray(m_debug_VAOS[DISPLAY_GRID]);
		glCheckError();
		// Load data into vertex buffers

		//vertex postion
		glBindBuffer(GL_ARRAY_BUFFER, m_debug_VBOS[DISPLAY_GRID]);
		glCheckError();
		glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(cgm::vec3), &position[0], GL_STATIC_DRAW);
		glCheckError();

		glEnableVertexAttribArray(0);
		glCheckError();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cgm::vec3), (GLvoid*)0);
		glCheckError();
		glBindVertexArray(0);
		glCheckError();

		// flag
		m_debug_flags[DISPLAY_GRID] = true;
		break;



	case DISPLAY_COLLIDERS:
		if (m_debug_flags[DISPLAY_COLLIDERS]) { //already set
			break;
		}
		else if (glIsVertexArray(m_debug_VAOS[DISPLAY_COLLIDERS]) == GL_TRUE) { // alredy set
			std::cerr << __FUNCTION__ << " ERROR: Debug option 'DISPLAY_COLLIDERS' is already set" << std::endl;
			break;
		}
		//SET THE SHADER, SET THE BUFFERS AND SET FLAG TO TRUE
		//shader
		m_debug_pshaders[DISPLAY_COLLIDERS] = &shader;
		m_debug_pshaders[DISPLAY_COLLIDERS]->use();
		//buffers
		
		// loop trouhg everty tile in the map
		for (int i = 0; i != m_tmap_ptr->height(); ++i) {
			for (int j = 0; j != m_tmap_ptr->width(); ++j) {
				//get the id of the tile in the map
				unsigned id = m_tmap_ptr->get_tile_id(0, i, j);
				Tile tile;
				//find out witch tileset this tile belongs to
				const Tileset * ptileset = nullptr;
				for (auto & tileset : m_tmap_ptr->get_tilesets()) {
					if (tileset.is_inside_set(id)) {
						ptileset = &tileset;
						break;
					}
				}
				
				if (ptileset) { // get the tile from the tileset
					tile = ptileset->get_tile(id);
					if (tile.m_is_obstacle) { // if it has a collider get the vertices position in the map to draw it
						std::cout << "tile with id = " << id << "is collidable" << std::endl;
						cgm::vec3 v0, v1, v2, v3;
						Rect rect = m_tmap_ptr->tile_wld_space_bounds(i, j);
						
						v0.x = rect.x;
						v0.y = rect.y - rect.height;

						v1.x = rect.x + rect.width;
						v1.y = rect.y - rect.height;

						v2.x = rect.x + rect.width;
						v2.y = rect.y;

						v3.x = rect.x;
						v3.y = rect.y;

						position.push_back(v0);
						position.push_back(v1);
						position.push_back(v2);
						position.push_back(v2);
						position.push_back(v3);
						position.push_back(v0);
						++m_debug_num_colliders; // remember how many tiles have colliders 
					}
				}
#ifndef NDEBUG
				else { //could not find tile 
					std::cerr << __FUNCTION__ << " ERROR: Could not find tileset containing tile with id = " << id << std::endl;
				}
#endif // !NDEBUG
			}
		}
		if (m_debug_num_colliders > 0) {
			// Create buffers/arrays
			glGenVertexArrays(1, &m_debug_VAOS[DISPLAY_COLLIDERS]);
			glCheckError();
			glGenBuffers(1, &m_debug_VBOS[DISPLAY_COLLIDERS]);
			glCheckError();

			glBindVertexArray(m_debug_VAOS[DISPLAY_COLLIDERS]);
			glCheckError();
			// Load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, m_debug_VBOS[DISPLAY_COLLIDERS]);
			glCheckError();
			glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(cgm::vec3), &position[0], GL_STATIC_DRAW);
			glCheckError();

			glEnableVertexAttribArray(0);
			glCheckError();
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cgm::vec3), (GLvoid*)0);
			glCheckError();
			glBindVertexArray(0);
			glCheckError();

			// flag
			m_debug_flags[DISPLAY_COLLIDERS] = true;
		}
		else {
			std::cout << __FUNCTION__ << ": No collidable tiles on the map to display in the DISPLAY_COLLIDERS debug mode" << std::endl;
		}
		break;
	}
#else
	std::cout << __FUNCTION__ << ": program build not in debug mode" << std::endl;
#endif


}

void Tile_map_renderer::switch_debug_option(Debug_options option, const bool value) 
{
#ifndef NTILE_MAP_DEBUG
	m_debug_flags[option] = value;
#else
	std::cout << __FUNCTION__ << ": program build not in debug mode" << std::endl;
#endif
}

void Tile_map_renderer::render(const gfx::Shader & shader)
{
	shader.use();
	//glCheckError();
	
	(m_tmap_ptr->get_tilesets())[0].get_texture().use();
	
	glBindVertexArray(m_VAO);
	//glCheckError();
	glDrawArrays(GL_TRIANGLES, 0 ,m_vertices_pos.size());
	//glCheckError();

	glBindVertexArray(0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//condition on debug flag
#ifndef NTILE_MAP_DEBUG
	if (m_debug_flags[DISPLAY_GRID] && (glIsVertexArray(m_debug_VAOS[DISPLAY_GRID]) == GL_TRUE) ) {
		m_debug_pshaders[DISPLAY_GRID]->use();
		glBindVertexArray(m_debug_VAOS[DISPLAY_GRID]);
		//glCheckError();
		glDrawArrays(GL_LINES, 0, (m_tmap_ptr-> width() + m_tmap_ptr-> height() + 2 ) * 2 );
		//glCheckError();
		glBindVertexArray(0);

	}
	if (m_debug_flags[DISPLAY_COLLIDERS] && (glIsVertexArray(m_debug_VAOS[DISPLAY_COLLIDERS]) == GL_TRUE) && (m_debug_num_colliders > 0) ) {
		m_debug_pshaders[DISPLAY_COLLIDERS]->use();
		glBindVertexArray(m_debug_VAOS[DISPLAY_COLLIDERS]);
		//glCheckError();
		glDrawArrays(GL_TRIANGLES, 0, 6 * m_debug_num_colliders);
		//glCheckError();
		glBindVertexArray(0);
	}
#endif
}