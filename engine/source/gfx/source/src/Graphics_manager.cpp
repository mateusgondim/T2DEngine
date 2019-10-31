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

bool gfx::Graphics_manager::init(int window_width, int window_height, const char * ptitle, float pixels_per_unit)
{
	m_pixels_per_unit = pixels_per_unit;
	m_tiles_per_screen_width = 16;
	m_tiles_per_screen_height = 15;

	m_batch_pool.alloc_pool(sizeof(Sprite_batch), 3, 4);

	// set the capacity of the vector<Sprite> and vector<sprite_batch*> to prevent 
    // dynamic allocation at runtime
	m_sprites.reserve(25);
	m_batches.reserve(3);

    // Create render window
    m_prender_window = Glfw_manager::create_window(window_width, window_height, ptitle);

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

// bool gfx::Graphics_manager::create_window(const std::int32_t width, const std::int32_t height, const char *ptitle)
// {
// 	m_pwindow = glfwCreateWindow(width, height, ptitle, NULL, NULL);
// 
// 	if (!m_pwindow) {
// 		shut_down();
// 		return false;
// 	}
// 	glfwMakeContextCurrent(m_pwindow);
// 	glfwSwapInterval(1);
// 
// 
// 	glewExperimental = GL_TRUE; // allow to use all of opengl core profile
// 
// 	if (glewInit() != GLEW_OK) {
// 		std::cerr << "Failed to initialize GLEW" << std::endl;
// 		shut_down();
// 		return false;
// 	}
// 
// 	glEnable(GL_SCISSOR_TEST);
// 
// 	// Now that we have a opengl context, create a batch object to start 
// 	void *pbatch_mem = m_batch_pool.get_element();
// 	Sprite_batch *pbach = new (pbatch_mem) Sprite_batch(6 * 25);
// 	m_batches.push_back(pbach);
// 
// 	m_is_initialized = true;
// 	return true;
// }


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

//	//initialize camera
//	float tile_wld_width = m_ptile_map->tile_width() / m_pixels_per_unit;
//	float tile_wld_height = m_ptile_map->tile_height() / m_pixels_per_unit;
//	math::vec2 map_origin = math::vec2(m_ptile_map->get_position().x, m_ptile_map->get_position().y);

//	m_camera.init(tile_wld_width, tile_wld_height, m_tiles_per_screen_width, m_tiles_per_screen_height, m_ptile_map->width(), m_ptile_map->height(), map_origin);
	
	//set background color
	math::vec3 color = m_ptile_map->get_background_color();
	set_clear_color(math::vec4(color.x, color.y, color.z, 1.0f));
	set_tile_map_renderer();
}


/* Load_shader: This function uses a pre allocated memory pool to construct a new  shader object. Using
   the name of the vertex shader file, a unique identifier is generated for the shader object, this identifier and
   a pointer to the shader object are stored in a std::map data structure. The function then returns the id of the shader  
 */
/*
gfx::Graphics_manager::shader_id gfx::Graphics_manager::load_shader(const char *v_shader_path, const char *f_shader_path) 
{
	//Use the vertex shader file path to generate unique id for the shader
	shader_id id = intern_string(v_shader_path);
	
	// Check if this id already exist
	std::map<shader_id, Shader*>::iterator it = m_shaders.find(id);
	if (it != m_shaders.end()) { // error 
		std::cerr << "ERROR(" << __FILE__ << "): A shader already exist with the same id, as the one generated by the path '" << v_shader_path << "' " << std::endl;
		return -1;
	}

	// there is no shader in the map with the generated id
	// request a memory block from the shader pool, to construct a new shader object
	void *pshader_mem = m_shader_pool.get_element();

	if (pshader_mem == nullptr) { // error allocating memory
		std::cerr << "ERROR(" << __FILE__ << "): Unable to allocate memory from the shader pool" << std::endl;
		return -1;
	}
	//construct the object on the returned memory block
	Shader *pshader = new (pshader_mem) Shader(v_shader_path, f_shader_path); 

	//save the pointer and the associated id on the map
	m_shaders[id] = pshader;

	return id;
}*/

/*
gfx::Shader *gfx::Graphics_manager::get_shader(gfx::Graphics_manager::shader_id id) 
{
	std::map<shader_id, Shader*>::iterator it = m_shaders.find(id);
	if (it != m_shaders.end()) {
		return it->second;
	}
	return nullptr;
}
*/

/*
void gfx::Graphics_manager::set_current_shader_program(gfx::Graphics_manager::shader_id id) 
{
	std::map<shader_id, Shader*>::iterator it = m_shaders.find(id);
	if (it != m_shaders.end()) {
		(it->second)->use();
		gfx_check_error();
	}
}*/

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

/* load_sprite_atlas: This function creates a new sprite_atlas object, using a pre allocated memory pool.
 * Using the sprite atlas file path, a unique identifier is generated for the atlas and stored with a pointer to
 * to the new object on a std::map. The id for the atlas is returned if the function is successful, -1 is returned otherwise.
 */

 /*
gfx::Graphics_manager::atlas_id gfx::Graphics_manager::load_sprite_atlas(const char *patlas_path) 
{
	//Use the atlas file path to generate a unique id 
	atlas_id id = intern_string(patlas_path);

	// check if there is already a atlas associated with this id
	std::map<atlas_id, Sprite_atlas*>::iterator it = m_atlases.find(id);
	if (it != m_atlases.end() ) { //error duplicate id!
		std::cerr << "ERROR(" << __FUNCTION__ << "): A sprite atlas already exist with the same id as the one generated by the path '" << patlas_path << "' " << std::endl;
		return -1;
	}

	// get a memory block from the atlas pool, to construct the new sprite_atlas object
	void *psprite_atlas_mem = m_atlas_pool.get_element();
	if (psprite_atlas_mem == nullptr) { // no memory left
		std::cerr << "ERROR(" << __FUNCTION__ << "): Unable to get memory block from the sprite_atlas memory pool" << std::endl;
		return -1;
	}

	//construct sprite_atlas object
	Sprite_atlas *psprite_atlas = new (psprite_atlas_mem) Sprite_atlas(patlas_path, id);

	//save the pair id-atlas on the map
	m_atlases[id] = psprite_atlas;

	return id;
}*/

/* get_sprite_component: This function creates a new sprite object using a pre allocated memory pool. The id of the texture_atlas 
 * associated with the sprite and the layer of the sprite, are passed as arguments. The new sprite object is stored in a std::vector
 * and the vector is sorted according to the layer and the texture_atlas id.
 */

/*
gfx::Sprite *gfx::Graphics_manager::get_sprite_component(const gfx::Graphics_manager::atlas_id id, const gfx::Graphics_manager::sprite_layer layer) 
{
	//check if there is a atlas with id = 'id'
	std::map<atlas_id, Sprite_atlas*>::iterator it = m_atlases.find(id);
	if (it == m_atlases.end()) { //error
		std::cerr << "ERROR(" << __FUNCTION__ << "): There is no sprite atlas with id = " << id << std::endl;
		return nullptr;
	}

	//get a pointer to the atlas
	Sprite_atlas *patlas = it->second;

	// Get a memory block from the sprite pool, to construct the new object
	void *psprite_mem = m_sprite_pool.get_element();
	if (psprite_mem == nullptr) {
		std::cerr << "ERROR(" << __FUNCTION__ << "): Unable to get memory block from the sprite pool" << std::endl;
		return nullptr;
	}

	//construct the sprite object
	Sprite *psprite = new (psprite_mem) Sprite(patlas, layer);

	//save the pointer in the sprite vector
	m_sprites.push_back(psprite);

	// sort the sprite vector by  layer and texture atlas
	std::sort(m_sprites.begin(), m_sprites.end(), sprite_sort);

	return psprite;
}*/

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

/*
void gfx::Graphics_manager::delete_sprite_component(Sprite *psprite) 
{
	// find the sprite pointer in the vector
	std::vector<Sprite*>::iterator it = std::lower_bound(m_sprites.begin(), m_sprites.end(), psprite->get_layer(), 
		[](const Sprite *psprite, const sprite_layer layer) {return psprite->get_layer() < layer; });
	for (; it != m_sprites.end(); ++it) {
		if (*it == psprite) {
			break;
		}
	}
	// if this sprite pointer is not in the vector
	if (it == m_sprites.end()) {
		std::cerr << "ERROR(" << __FUNCTION__ << "): Invalid Sprite pointer" << std::endl;
		return;
	}

	//call the sprite destructor
	(*it)->~Sprite();
	
	// release the memory block back to the pool
	void *psprite_mem = static_cast<void*>(*it);
	m_sprite_pool.free_element(psprite_mem);

	//remove the sprite pointer from the vector
	m_sprites.erase(it);

	// sort the vector
	std::sort(m_sprites.begin(), m_sprites.end(), sprite_sort);
}
*/

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

/*
void gfx::Graphics_manager::delete_texture_atlas(const atlas_id id) 
{
	// get the atlas by id
	std::map<atlas_id, Sprite_atlas*>::iterator it = m_atlases.find(id);

	//if this id is invalid
	if (it == m_atlases.end()) {
		std::cerr << "ERROR(" << __FUNCTION__ << "): Invalid atlas id" << std::endl;
		return;
	}

	// call the atlas desctructor
	Sprite_atlas *patlas = it->second;
	patlas->~Sprite_atlas();

	// give back the memory block to the atlas pool
	void *patlas_mem = static_cast<void*>(patlas);
	m_atlas_pool.free_element(patlas_mem);

	//remove the id and string entry from the global string table
	remove_string_id_entry(id);

	//remove the <id,sprite_atlas*> pair from the map
	m_atlases.erase(id);
}
*/

/*
void gfx::Graphics_manager::delete_shader(const shader_id id) 
{
	// get the shader object by id
	std::map<shader_id, Shader*>::iterator it = m_shaders.find(id);

	//if this id is invalid
	if (it == m_shaders.end()) {
		std::cerr << "ERROR(" << __FUNCTION__ << "): Invalid shader id" << std::endl;
		return;
	}

	// call the shader desctructor
	Shader *pshader = it->second;
	pshader->~Shader();

	//give back the memory block to the shader pool
	void *pshader_mem = static_cast<void*>(pshader);
	m_shader_pool.free_element(pshader_mem);

	//remove the id and string from the global string table
	remove_string_id_entry(id);

	//remove the pair <shader_id, shader*> from the map
	m_shaders.erase(id);
}*/

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
		
		/*
		texture_id tex_id = tileset.get_texture_id();

		//check if there is already a texture with this id on the map
		std::map<texture_id, Texture_2d*>::iterator it = m_textures.find(tex_id);

		if (it != m_textures.end()) {
			std::cerr << "ERROR(" << __FUNCTION__ << "): There is already a texture with id = " << tex_id << " in the map" << std::endl;
			return;
		}
		
		// get the texture file path from id
		const char *ptexture_file_path = get_string_by_id(tex_id);

		//get a memory block from the texture pool
		void *ptex_mem = m_texture_pool.get_element();

		//check if there is memory left
		if (ptex_mem == nullptr) {
			std::cerr << "ERROR(" << __FUNCTION__ << "): " << "There is no memory left in the Texture memory pool " << std::endl;
			return;
		}

		//construct the texture_2d object
		Texture_2d *ptexture = new (ptex_mem) Texture_2d(ptexture_file_path);

		// add texture to map
		m_textures[tex_id] = ptexture;
		*/
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
	/*
	// destruct the shader objects and remove their ids from the table
	for (std::map<shader_id, Shader*>::iterator it = m_shaders.begin(); it != m_shaders.end(); ++it) {
		shader_id id	 =  it->first;
		Shader *pshader	 =  it->second;

		remove_string_id_entry(id); 
		pshader->~Shader();

		//release the memory block
		m_shader_pool.free_element(static_cast<void*>(pshader));
	}
	// deallocate the shader pool
	//m_shader_pool.realease_pool_mem();

	//destruct the Sprite_atlas objects and remove their ids from the table
	for (std::map<atlas_id, Sprite_atlas*>::iterator it = m_atlases.begin(); it != m_atlases.end(); ++it) {
		atlas_id       id      =  it->first;
		Sprite_atlas  *patlas  =  it->second;

		remove_string_id_entry(id);
		patlas->~Sprite_atlas();

		//release the pool memory block
		m_atlas_pool.free_element(static_cast<void*>(patlas));
	}
	//deallocate sprite atlas pool
	//m_atlas_pool.realease_pool_mem();

	//destruct the Sprite objects
	for (std::vector<Sprite*>::iterator it = m_sprites.begin(); it != m_sprites.end(); ++it) {
		Sprite *psprite = *it;
		psprite-> ~Sprite();

		//release the pool memory block
		m_sprite_pool.free_element(static_cast<void*>(psprite));

	}
	//deallocate sprite pool
	//m_sprite_pool.realease_pool_mem();
	*/
	
	//destruct the batch objects 
	//destroy the tile_map batch and, give the memory back to the pool
	m_pmap_batch->~Sprite_batch();
	m_batch_pool.free_element(static_cast<void*>(m_pmap_batch));

	//destroy the sprite Batches and deallocate the pool 
	for (std::vector<Sprite_batch*>::iterator it = m_batches.begin(); it != m_batches.end(); ++it) {
		Sprite_batch *pbatch = *it;
		pbatch->~Sprite_batch();

		//release the pool memory block
		m_batch_pool.free_element(static_cast<void*>(pbatch));
	}
	//deallocate Sprite batch pool
	//m_batch_pool.realease_pool_mem();
	
	/*
	//destroy the texture objects
	for (std::map<texture_id, Texture_2d*>::iterator it = m_textures.begin(); it != m_textures.end(); ++it) {
		Texture_2d *ptexture = it->second;
		ptexture->~Texture_2d();

		//release the pooll memory block
		m_texture_pool.free_element(static_cast<void*>(ptexture));
	}
	*/
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