#ifndef _GRAPHICS_MANAGER_HPP
#define _GRAPHICS_MANAGER_HPP

#include <cstdint>
#include <map>
#include <vector>

#include "Pool_allocator.hpp"
#include "string_id.hpp"
//#include "Camera_2d.hpp"

#if GRAPHICS_CONSOLE_DEBUG_ENABLE

#define gfx_check_error() check_error(__FILE__, __LINE__); 

#else

#define gfx_check_error()

#endif

struct GLFWwindow;
class  Tile_map;
namespace gfx { class Sprite; class Shader; class Sprite_batch; }
namespace math { struct vec4; }

namespace gfx {
	enum class GFX_ENUMS { SRC_ALPHA, ONE_MINUS_SRC_ALPHA, BLEND, COLOR_BUFFER_BIT, DEPTH_BUFFER_BIT };
	class Graphics_manager {
		typedef		string_id		atlas_id;
		typedef		string_id       texture_id;
		typedef		string_id		shader_id;
		typedef		std::uint8_t	sprite_layer;

		typedef     void (*error_callback_ptr) (int, const char*);
		typedef     void (*key_callback_ptr)   (int, int, int, int);
	public:
			        Graphics_manager();
					~Graphics_manager() = default;
		//initialization functions
		void		init(const std::uint8_t context_version_major, const std::uint8_t context_version_minor, const float pixels_per_unit = 16.0f);
		bool        create_window(const std::int32_t width, const std::int32_t height, const char *ptitle);
		
		// Incapsulation of OpenGl and GLFW  functions
		bool        window_should_close() const;
		void        set_error_callback(error_callback_ptr fptr);
		void		set_key_callback(key_callback_ptr fptr);
		
		void		get_framebuffer_size(std::int32_t * pwidth, std::int32_t *pheight);
		void        set_viewport(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height);
		//Camera_2d & get_camera();

		void		set_clear_color(const math::vec4 & color);
		void		set_blend_func(); // CHANGE THIS!! PASS PARAMETERS
		void        graphics_enable(const GFX_ENUMS & gfx_enum);// GRAPHICS_ENUM
		void        clear_color_buffers();

		void		shut_down();
		
		// shader operations
		//shader_id	load_shader(const char *v_shader_path, const char *f_shader_path);
		//Shader      *get_shader(shader_id id);
		//void		 set_current_shader_program(shader_id id);
		void         uniform_matrix4fv(std::int32_t location, std::int32_t count, bool is_transposed, const float *pvalue);
		void		 uniform_1f(std::int32_t location, float v0);

		void		set_sprite_shader(gfx::Shader *pshader);
		void		set_tile_map_shader(gfx::Shader *pshader);
		void		add_sprite_to_render(gfx::Sprite *psprite);
		void		rem_sprite_from_batch(const gfx::Sprite *psprite);

		//atlas_id	load_sprite_atlas(const char *patlas_path);

		//Tile map operations
		void		set_tile_map(Tile_map *ptile_map);

		void	    set_tiles_per_screen_width(const float width)  { m_tiles_per_screen_width = width; }
		void		set_tiles_per_screen_height(const float height) { m_tiles_per_screen_height = height; }
        float       get_pixels_per_wld_unit() const { return m_pixels_per_unit; }
        float       get_tiles_per_screen_width() const { return m_tiles_per_screen_width; }
        float       get_tiles_per_screen_height() const { return m_tiles_per_screen_height; }
		//Sprite      *get_sprite_component(const atlas_id id, const sprite_layer layer);
		//void		 delete_sprite_component(Sprite *psprite);
		//void		 delete_texture_atlas(const atlas_id id);
		//void		 delete_shader(const shader_id id);

		void         render();
		
	private:
		void        set_tile_map_renderer();
		static  void key_callback(GLFWwindow *pwindow, int key, int scancode, int action, int mods);
		void		check_error(const char *pfile_name, std::int32_t line) const;
		// Global Graphics objects 
		//std::map<atlas_id, Sprite_atlas*>			m_atlases;
		//std::map<shader_id, Shader*>				m_shaders;
		//std::map<texture_id, Texture_2d*>           m_textures;
		//Camera_2d									m_camera;

		std::vector<Sprite_batch*>					m_batches;
		std::vector<Sprite*>						m_sprites;
		//std::map<controller_id, Animator_controller*> m_animator_controllers;

		// memory allocators
		//Pool_allocator								m_atlas_pool;
		//Pool_allocator                              m_shader_pool;
		mem::Pool_allocator                              m_batch_pool;
		//Pool_allocator								m_sprite_pool;
		//Pool_allocator								m_texture_pool; // for the tile_map textures...
		//Pool_allocator								m_anim_controller_pool

		// Data members to render Tile_map
		Tile_map								   *m_ptile_map; // pointer to tile_map data object to render
		Sprite_batch							   *m_pmap_batch; // pointer to batch contaning the map's vertices 

		gfx::Shader									*m_psprite_shader; // the id for the shader used to render the sprites
		gfx::Shader									*m_ptile_map_shader; // the id for the shader used to render the Tile map

		// Opengl Context data
		GLFWwindow		*m_pwindow; // GLFW window
		//Opengl's context version
		std::uint8_t	m_context_version_major; 
		std::uint8_t	m_context_version_minor;

		static key_callback_ptr s_key_callback;

		float			m_pixels_per_unit;
		float           m_tiles_per_screen_width;
		float           m_tiles_per_screen_height;
		bool			m_is_initialized;

	};
	extern  Graphics_manager g_graphics_mgr;
	bool	sprite_sort(const gfx::Sprite *lhs, const gfx::Sprite *rhs);
}

#endif // !_GRAPHICS_MANAGER_HPP
