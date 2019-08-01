#ifndef _LEVEL_MANAGER_HPP
#define _LEVEL_MANAGER_HPP
#include <vector>
#include <stdint.h>
#include "Path.hpp"
#include "Timer.hpp"
#include "Camera_2d.hpp"
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
                gom::Camera_2d & get_camera();
                bool is_game_clock_paused() const { return m_timer.is_paused(); }
                //void next_level();
        private:
                void            load_level_objects();
                void            instantiate_level_objects();
                Path*                                           m_presources_path = nullptr;
                gom::Camera_2d                                  m_camera;
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
                float                                           m_target_aspect_ratio;
                float                                           m_curr_aspect_ratio;
                int                                             m_prev_vport_width;
                int                                             m_prev_vport_height;
                int                                             m_curr_vport_width;
                int                                             m_curr_vport_height;
                int32_t                                         m_sprites_view_loc;
                int32_t                                         m_tile_map_view_loc;
                bool                                            m_should_restart;

        };
        extern Level_manager g_level_mgr;

        inline gom::Camera_2d & Level_manager::get_camera()
        {
                return m_camera;
        }
}
#endif // !_LEVEL_MANAGER_HPP
