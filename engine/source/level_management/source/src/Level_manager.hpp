#ifndef _LEVEL_MANAGER_HPP
#define _LEVEL_MANAGER_HPP
#include <vector>
#include <stdint.h>
#include "Path.hpp"
#include "Timer.hpp"
#include "Game_object_data.hpp"

namespace gfx { class Shader; }
class Tile_map;

namespace level_management
{
        class Level_manager final {
        public:
                Level_manager() = default;
                ~Level_manager();
                void load(Path & resources_path, Tile_map *ptile_map);
                void init();
              //  void shut_down();
                void tick();
                void restart();
                bool is_game_clock_paused() const { return m_timer.is_paused(); }
                //void next_level();
        private:
                void            load_objects_data();
                void            instantiate_level_objects();
                Path*                                           m_presources_path = nullptr;
                std::vector<Game_object_data>                   m_level_data;
                Tile_map*                                       m_ptile_map;
                gfx::Shader*                                    m_psprite_shader;
                gfx::Shader*                                    m_pmap_shader;
                Timer                                           m_timer;
                uint64_t                                        m_last_time_cycles;
                uint64_t                                        m_curr_time_cycles;
                float                                           m_delta_time_seconds;
                float                                           m_lag;
                static const float                              m_dt;
                int32_t                                         m_sprites_view_loc;
                int32_t                                         m_tile_map_view_loc;
                bool                                            m_should_restart;

        };
        extern Level_manager g_level_mgr;
}
#endif // !_LEVEL_MANAGER_HPP
