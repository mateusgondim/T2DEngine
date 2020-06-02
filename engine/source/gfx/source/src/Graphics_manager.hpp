#ifndef _GRAPHICS_MANAGER_HPP
#define _GRAPHICS_MANAGER_HPP

#include <cstdint>
#include <map>
#include <vector>

#include "Pool_allocator.hpp"
#include "string_id.hpp"

#if GRAPHICS_CONSOLE_DEBUG_ENABLE

#define gfx_check_error() check_error(__FILE__, __LINE__); 

#else

#define gfx_check_error()

#endif

class  Tile_map;
namespace gfx { class Window; class Sprite; class Shader; class Sprite_batch; }
namespace math { struct vec4; }

namespace gfx {
	enum class GFX_ENUMS { SRC_ALPHA, ONE_MINUS_SRC_ALPHA, BLEND, COLOR_BUFFER_BIT, DEPTH_BUFFER_BIT };
	class Graphics_manager {
		typedef		string_id		atlas_id;
		typedef		string_id       texture_id;
		typedef		string_id		shader_id;
		typedef		std::uint8_t	sprite_layer;
	public:
            Graphics_manager();
            ~Graphics_manager() = default;
		//initialization functions
            bool		init(int window_width, int window_height, float viewport_scale,
                             const char * ptitle);
            Window  *   get_render_window();
		
		void        set_viewport(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height);

        void        set_pixels_per_wld_unit(const float px_per_wld_unit)
        { 
                m_pixels_per_unit = px_per_wld_unit; 
        }
		void		set_clear_color(const math::vec4 & color);
		void		set_blend_func(); // CHANGE THIS!! PASS PARAMETERS
		void        graphics_enable(const GFX_ENUMS & gfx_enum);// GRAPHICS_ENUM
		void        clear_color_buffers();

		void		shut_down();
		
		void         uniform_matrix4fv(std::int32_t location, std::int32_t count, bool is_transposed, const float *pvalue);
		void		 uniform_1f(std::int32_t location, float v0);

		void		set_sprite_shader(gfx::Shader *pshader);
		void		set_tile_map_shader(gfx::Shader *pshader);
		void		add_sprite_to_render(gfx::Sprite *psprite);
		void		rem_sprite_from_batch(const gfx::Sprite *psprite);


		//Tile map operations
		void		set_tile_map(Tile_map *ptile_map);

        void	    set_tiles_per_screen_width(const float width) { m_tiles_per_screen_width = width; }
		void		set_tiles_per_screen_height(const float height) { m_tiles_per_screen_height = height; }
        float       get_pixels_per_wld_unit() const { return m_pixels_per_unit; }
        float       get_tiles_per_screen_width() const { return m_tiles_per_screen_width; }
        float       get_tiles_per_screen_height() const { return m_tiles_per_screen_height; }

		void         render();
        void         swap_and_poll() const;
		
	private:
            void        set_tile_map_renderer();
            void		check_error(const char *pfile_name, std::int32_t line) const;

            std::vector<Sprite_batch*>					m_batches;
            std::vector<Sprite*>						m_sprites;

            // memory allocators
            mem::Pool_allocator                              m_batch_pool;

            // Data members to render Tile_map
            Tile_map								   *m_ptile_map; // pointer to tile_map data object to render
            Sprite_batch							   *m_pmap_batch; // pointer to batch contaning the map's vertices 

            gfx::Shader									*m_psprite_shader; // the id for the shader used to render the sprites
            gfx::Shader									*m_ptile_map_shader; // the id for the shader used to render the Tile map

            Window *m_prender_window;

            float			m_pixels_per_unit;
            float           m_tiles_per_screen_width;
            float           m_tiles_per_screen_height;
            bool			m_is_initialized;
	};

	extern  Graphics_manager g_graphics_mgr;
	bool	sprite_sort(const gfx::Sprite *lhs, const gfx::Sprite *rhs);
}

#endif // !_GRAPHICS_MANAGER_HPP
