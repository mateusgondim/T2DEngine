#include "Graphics_manager.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Window.hpp"
#include "Glfw_manager.hpp"
#include "string_id.hpp"
#include "Shader.hpp"
#include "Sprite.hpp"
#include "Sprite_atlas.hpp"
#include "Sprite_batch.hpp"
#include "Resource.hpp"
#include "Texture_2d_manager.hpp"
#include "Texture_2d.hpp"
#include "Sprite_atlas_manager.hpp"

#include "Tileset.hpp"
#include "Tile_map.hpp"
#include "Vertex1P1C1UV.hpp"

#include "vec4.hpp"

#include <cstdint>
#include <iostream>
#include <string>
#include <algorithm>

gfx::Graphics_manager gfx::g_graphics_mgr;

gfx::Graphics_manager::Graphics_manager() : m_is_initialized(false) {}

bool gfx::Graphics_manager::init(int window_width, int window_height, float viewport_scale,
                                 const char * ptitle)
{
	m_pixels_per_unit = 0.0f;
	m_tiles_per_screen_width = 16;
	m_tiles_per_screen_height = 15;
    m_ptile_map = nullptr;

	m_batch_pool.alloc_pool(sizeof(Sprite_batch), 3, 4);

	// set the capacity of the vector<Sprite> and vector<sprite_batch*> to prevent 
    // dynamic allocation at runtime
	m_sprites.reserve(25);
	m_batches.reserve(3);

    // Create render window
    m_prender_window = Glfw_manager::create_window(window_width, window_height, viewport_scale,
                                                   ptitle);

    if (!m_prender_window->is_initialized()) {
            shut_down();
            return false;
    }

    Glfw_manager::make_context_current(m_prender_window);
    Glfw_manager::swap_interval(1);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            shut_down();
            return false;
    }

    glEnable(GL_SCISSOR_TEST);

    // Now that we have a opengl context, create a batch object to start 
    void *pbatch_mem = m_batch_pool.get_element();
    Sprite_batch *pbach = new (pbatch_mem) Sprite_batch(6 * 25);
    m_batches.push_back(pbach);

    m_is_initialized = true;
    return true;
}

gfx::Window * gfx::Graphics_manager::get_render_window()
{
        return m_prender_window;
}


void gfx::Graphics_manager::set_clear_color(const math::vec4 & color) 
{
	glClearColor(color.x, color.y, color.z, color.w);
	gfx_check_error();
}

void gfx::Graphics_manager::clear_color_buffers()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gfx_check_error();
}

void gfx::Graphics_manager::graphics_enable(const gfx::GFX_ENUMS & gfx_enum) 
{
	switch (gfx_enum) {
	case GFX_ENUMS::BLEND:
		glEnable(GL_BLEND);
		gfx_check_error();
		break;
	default:
		std::cerr << "ERROR(" << __FUNCTION__ << "): INVALID ENUM FOR OPERATION " << std::endl;
		break;
	}
}

//CHANGE THIS!!!!!!!!!!!! PASS PARAMETERS!!!!!!!!!!
void gfx::Graphics_manager::set_blend_func() 
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gfx_check_error();
}


void gfx::Graphics_manager::set_viewport(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height) 
{
	if (!m_is_initialized) {
		return;
	}

	glViewport(x, y, width, height);
	glScissor(x, y, width, height);
	
	//Check opengl's error in GRAPHICS_CONSOLE_DEBUG_MODE
	gfx_check_error();
}

void gfx::Graphics_manager::set_tile_map(Tile_map *ptile_map) 
{
        m_ptile_map = ptile_map;
	
        //set background color
        math::vec3 color = m_ptile_map->get_background_color();
        set_clear_color(math::vec4(color.x, color.y, color.z, 1.0f));
        set_tile_map_renderer();
}


void gfx::Graphics_manager::uniform_matrix4fv(std::int32_t location, std::int32_t count, bool is_transposed, const float *pvalue) 
{
	if (!m_is_initialized) {
		//DISPLAY INFORMATION!!!!!!!
		return;
	}
	GLboolean transpose = (is_transposed) ? (GL_TRUE) : (GL_FALSE);
	glUniformMatrix4fv(location, count, transpose, pvalue);

	//Check opengl's error in GRAPHICS_CONSOLE_DEBUG_MODE
	gfx_check_error();
}

void gfx::Graphics_manager::uniform_1f(std::int32_t location, float v0) 
{
	if (!m_is_initialized) {
		//DISPLAY INFORMATION!!!!!!!
		return;
	}
	
	glUniform1f(location, v0);
	
	//Check opengl's error in GRAPHICS_CONSOLE_DEBUG_MODE
	gfx_check_error();
}

void gfx::Graphics_manager::set_sprite_shader(gfx::Shader *pshader) 
{
	m_psprite_shader = pshader;
}

void gfx::Graphics_manager::set_tile_map_shader(gfx::Shader *pshader) 
{
	m_ptile_map_shader = pshader;
}

void gfx::Graphics_manager::add_sprite_to_render(gfx::Sprite *psprite) 
{
	//save the pointer in the sprite vector
	m_sprites.push_back(psprite);

	// sort the sprite vector by  layer and texture atlas
	std::sort(m_sprites.begin(), m_sprites.end(), sprite_sort);
}


void gfx::Graphics_manager::rem_sprite_from_batch(const gfx::Sprite *psprite) 
{
	//find the interval containing the sprite 
	std::vector<gfx::Sprite*>::iterator it = std::lower_bound(m_sprites.begin(), m_sprites.end(), psprite, sprite_sort);
	if (it != m_sprites.end()) {
		for (; it != m_sprites.end(); ++it) {
			if (*it == psprite) {
				break;
			}
		}
		if (it != m_sprites.end()) {
			//remove sprite
			m_sprites.erase(it);
		}
	}
}


/* sprite_sort: Comparison function used by the std::sort algorithm to sort the sprites
 *   based on layer first. those with the same layer, are sorted based on the sprite atlas id.
 */
bool gfx::sprite_sort(const gfx::Sprite *lhs, const gfx::Sprite *rhs) 
{
	if (lhs->get_layer() == rhs->get_layer()) {
		// if they belong to the same layer, sort them by texture atlas
		return lhs->get_atlas()->get_id() < rhs->get_atlas()->get_id();
	}
	else {
		return lhs->get_layer() < rhs->get_layer();
	}
}

/* render: Render all the sprites in vector, grouping those with the samer layer and the same texture atlas, 
 * on a single batch to be drawn using a single OpenGL's drawcall.
 */
void gfx::Graphics_manager::render()
{
	clear_color_buffers();

	//render the tile map, layer by layer
	//set_current_shader_program(m_tile_map_shader_id); 
	m_ptile_map_shader->use();

	texture_id text_id = ((m_ptile_map->get_tilesets())[0]->get_texture_id());

	//Texture_2d *ptexture = m_textures[text_id];
	Texture_2d   *ptexture = static_cast<Texture_2d*>(g_texture_2d_mgr.get_by_id(text_id));
	ptexture->use();
	m_pmap_batch->render();
    glBindTexture(GL_TEXTURE_2D, 0);

	//render the sprites
	//set_current_shader_program(m_sprite_shader_id);
	m_psprite_shader->use();

	if (!m_sprites.empty()) { // if there is at least one sprite on the vector	
	
	   // get a pointer to the first sprite in the vector
		Sprite       *psprite = m_sprites[0];

      // get the layer and the atlas id for the first sprite
		sprite_layer  layer =  psprite->get_layer();
		atlas_id      a_id  =  psprite->get_atlas()->get_id();

		// get a batch to render the sprites
		Sprite_batch *pbatch = m_batches[0];

		//MAYBE CHECK IF THE THERE IS A BATCH ALLOCATED, I.E, IF m_batches[0] != nullptr

		// add the first sprite to the batch if active
		if (psprite->is_active()) {
			pbatch->add(psprite);
		}
		// for each sprite on the vector
		std::vector<Sprite*>::iterator it = m_sprites.begin();
		++it;
		for (; it != m_sprites.end(); ++it) {
			//check if the current sprite belongs to the batch
			if (  (layer == (*it)->get_layer()) && (a_id == (*it)->get_atlas()->get_id()) ) {
				// same layer, same atlas_id, add it to the current batch if active
				if ((*it)->is_active()) {
					pbatch->add(*it);
				}
			}
			else { 
				 /* This sprite belongs to another batch. Because the vector is sorted, 
				  * there is no other sprite with the same layer and atlas id, so this batch is done.
				  */ 
				
				 //bind the sprite_atlas texture
				//Maybe we should check if the id is correct, if is not, we can use a default texture, that draws something like a red square...
				//Sprite_atlas *patlas = (m_atlases.find(a_id))->second;
				Sprite_atlas *pbatch_atlas = static_cast<Sprite_atlas*>(g_sprite_atlas_mgr.get_by_id(a_id));
				pbatch_atlas->get_texture()->use();

				//draw the sprites in the batch
				pbatch->render();

				// set the new layer and atlas id for the next batch
				if (layer != (*it)->get_layer()) {
					layer = (*it)->get_layer();
				}
				if (a_id != (*it)->get_atlas()->get_id()) {
					a_id = (*it)->get_atlas()->get_id();
				}

				// start a new batch
				//add if active
				if ((*it)->is_active()) {
					pbatch->add(*it);
				}
			}
		}

		// Draw the sprites in the last batch
		//Sprite_atlas *patlas = (m_atlases.find(a_id))->second;
		//patlas->get_texture().use();
		Sprite_atlas *pbatch_atlas = static_cast<Sprite_atlas*>(g_sprite_atlas_mgr.get_by_id(a_id));
		pbatch_atlas->get_texture()->use();

		pbatch->render();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void gfx::Graphics_manager::swap_and_poll() const
{
        m_prender_window->swap_buffers();
        Glfw_manager::poll_events();
}

void gfx::Graphics_manager::unload_map_rendering_data()
{
        if (!m_ptile_map) {
                return;
        }
        // unload textures used by the tilesets
        for (auto & tileset : m_ptile_map->get_tilesets()) {
                g_texture_2d_mgr.unload(tileset->get_texture_id());
        }

        // destroy the tile_map batch and, give the memory back to the pool
        m_pmap_batch->~Sprite_batch();
        m_batch_pool.free_element(static_cast<void*>(m_pmap_batch));

        m_ptile_map = nullptr;
}


/*Set_tile_map_renderer: Set up the Opengl's buffers to render the tile_map and,
 * allocates textures used by the map's tilesets.
 */
void gfx::Graphics_manager::set_tile_map_renderer() 
{
	//load the textures used by the tilesets
	for (auto & tileset : m_ptile_map->get_tilesets()) {
		/* Each map layer should use the same tileset
		 * so each tilese here, is used by a specific layer of the map
		 */
		
		//get the file path for the texture of this tileset
		std::string file_path = tileset->get_texture_file_path();

		//get the name of texture file to generate the resource unique
		auto pos = (file_path.find_last_of('/') == std::string::npos) ? (file_path.find_last_of('\\')) : (file_path.find_last_of('/'));
		++pos;
		std::string file_name = file_path.substr(pos);

		//load the texture resource
		rms::Resource *resource = g_texture_2d_mgr.load(file_name.c_str(), file_path.c_str());
		if (resource != nullptr) {
			tileset->set_texture_id(resource->get_id());
		}
		else {
			std::cerr << "ERROR(" << __FILE__ << "): Could not load texture " << file_path << std::endl;
		}
	}

	// get the memory block from the  batch pool
	void *pbatch_mem = m_batch_pool.get_element();

	// check if there memory left in the pool
	if (pbatch_mem == nullptr) {
		std::cerr << "ERROR(" <<  __FUNCTION__ << "): No memory left in batch pool" << std::endl;
		return;
	}

	//construct the batch object to render the tile map
	m_pmap_batch = new (pbatch_mem) Sprite_batch(m_ptile_map->width() * m_ptile_map->height() * 6, true);

	std::vector<Vertex1P1C1UV> vertices;
	
	// for each tile on the map
	for (int i = 0; i != m_ptile_map->height(); ++i) {
		for (int j = 0; j != m_ptile_map->width(); ++j) {
			// get the world space coordinates of the tile
			Vertex1P1C1UV vertex1, vertex2, vertex3, vertex4;
			math::Rect rect = m_ptile_map->tile_wld_space_bounds(i, j);

			vertex1.m_pos = math::vec3(rect.x, rect.y, 0.0f);
			vertex2.m_pos = math::vec3(rect.x, rect.y - rect.height, 0.0f);
			vertex3.m_pos = math::vec3(rect.x + rect.width, rect.y - rect.height, 0.0f);
			vertex4.m_pos = math::vec3(rect.x + rect.width, rect.y, 0.0f);
			
			vertex1.m_col = vertex2.m_col = vertex3.m_col = vertex4.m_col = math::vec4(1.0f, 0.0f, 0.0f, 0.0f);

			// get the uv coordinates of the tile
			std::uint32_t tile_id = m_ptile_map->get_tile_id(0, i, j);

			if (tile_id == 0) {//empty tile
				continue;
			}

			//find witch tileset has the tile width id
			const Tileset * ptileset = nullptr;
			for (auto & tileset : m_ptile_map->get_tilesets()) {
				if (tileset->is_inside_set(tile_id)) {
					ptileset = tileset;
					break;
				}
			}
			if (ptileset) {
				ptileset->get_text_coord(tile_id, vertex1.m_uv, vertex2.m_uv, vertex3.m_uv, vertex4.m_uv);
			}
#ifndef NDEBUG
			else {
				std::cerr << __FUNCTION__ << " ERROR: Could not find tileset containing tile with id = " << tile_id << std::endl;
			}
#endif // !NDEBUG

			vertices.push_back(vertex1);
			vertices.push_back(vertex2);
			vertices.push_back(vertex3);

			vertices.push_back(vertex3);
			vertices.push_back(vertex4);
			vertices.push_back(vertex1);
		}
	}

	//add the vertices to the batch
	m_pmap_batch->add(vertices);

}

//shut_down
void gfx::Graphics_manager::shut_down() 
{
	// destruct the batch objects 
	// destroy the tile_map batch and, give the memory back to the pool
	m_pmap_batch->~Sprite_batch();
	m_batch_pool.free_element(static_cast<void*>(m_pmap_batch));

	// destroy the sprite Batches and deallocate the pool 
	for (std::vector<Sprite_batch*>::iterator it = m_batches.begin(); it != m_batches.end(); ++it) {
		Sprite_batch *pbatch = *it;
		pbatch->~Sprite_batch();

		//release the pool memory block
		m_batch_pool.free_element(static_cast<void*>(pbatch));
	}
	
    delete m_prender_window;
    m_prender_window = nullptr;
	m_is_initialized = false;
}

void gfx::Graphics_manager::check_error(const char *pfile_name, std::int32_t line) const 
{
	GLenum error_code;
	while ((error_code = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (error_code)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cerr << error << " | " << pfile_name << " (" << line << ")" << std::endl;
	}
}