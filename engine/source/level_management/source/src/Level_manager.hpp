#ifndef _LEVEL_MANAGER_HPP
#define _LEVEL_MANAGER_HPP
#include <vector>
#include <string>
#include <stdint.h>
#include "Path.hpp"
#include "Timer.hpp"

namespace gfx { class Shader; }
class Tile_map;

namespace level_management
{
    class Level_manager final {
    public:
        Level_manager() = default;
        ~Level_manager() = default;

        void load_resident_data(const char * pplevels[], const uint32_t num_levels);
        void request_next_level();
        void request_level(const uint32_t level_index);

        void shut_down();
        void tick();
        void request_restart();
        bool is_game_clock_paused() const { return m_timer.is_paused(); }
    private:
        void init();
        void instantiate_level_objects();
        void unload_current_level();
        void load_requested_level();
        void restart_level();

        Tile_map*                           m_ptile_map;
        gfx::Shader*                        m_psprite_shader;
        gfx::Shader*                        m_pmap_shader;
        Timer                               m_timer;
        uint64_t                            m_last_time_cycles;
        uint64_t                            m_curr_time_cycles;
        float                               m_delta_time_seconds;
        float                               m_lag;
        static const float                  m_dt;
        int32_t                             m_sprites_view_loc;
        int32_t                             m_tile_map_view_loc;
        std::vector<std::string>            m_levels;
        std::vector<std::string>::size_type m_current_level;
        bool                                m_should_restart;
        bool                                m_should_load_next_level;

    };
    extern Level_manager g_level_mgr;
}
#endif // !_LEVEL_MANAGER_HPP
