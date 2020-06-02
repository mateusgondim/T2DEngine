#include "Text.hpp"

#include "Widget.hpp"
#include "Canvas.hpp"
#include "UI_manager.hpp"
#include "Rect.hpp"
#include "vec2.hpp"
#include "vec4.hpp"
#include "mat4.hpp"
#include "crc32.hpp"
#include "Sprite_atlas.hpp"
#include "Graphics_manager.hpp"

#include <string>
#include <vector>
#include <utility>

namespace ui
{
        Text::Text(Canvas & parent_canvas, const std::string & msg, const std::size_t obj_sz,
                   const float scale_factor, const int characters_space,
                   const int space_character_sz) :
                Widget(parent_canvas, math::Rect(), obj_sz), m_scale_factor(scale_factor),
                m_characters_space(characters_space), m_space_character_sz(space_character_sz),
                m_characters_ids(msg.size()), m_msg(msg)
        {
                for (unsigned i = 0; i != msg.size(); ++i) {
                        m_characters_ids[i] = get_crc32(m_msg[i]);
                }
                set_obj_space_rect();
        }

        void Text::set_obj_space_rect()
        {
                if (m_characters_ids.empty()) {
                        return;
                }

                const float pixels_per_wld_unit = gfx::g_graphics_mgr.get_pixels_per_wld_unit();
                const float space_char_sz = (m_space_character_sz / pixels_per_wld_unit * m_scale_factor);
                const float space_between_chars = (m_characters_space / pixels_per_wld_unit * m_scale_factor);

                float height = 0.0f;
                float width = 0.0f;

                const gfx::Sprite_atlas * atlas = m_pparent_canvas->get_atlas();
                for (std::size_t i = 0; i != m_characters_ids.size(); ++i) {
                        const gfx::Atlas_image & image = atlas->get_image_data(m_characters_ids[i]);
                        float char_width_wld_units = (image.m_width * m_scale_factor) / pixels_per_wld_unit;
                        float char_height_wld_units = (image.m_height * m_scale_factor) / pixels_per_wld_unit;

                        // check if it is a a space character
                        if (image.m_width == 0 && image.m_height == 0) {
                                width += space_char_sz;;
                                continue;
                        }

                        width += char_width_wld_units + space_between_chars; 
                        if (height < char_height_wld_units) {
                                height = char_height_wld_units;
                        }
                }
                width -= space_between_chars;

                // account for floating point error
                width += (1 * m_scale_factor) / pixels_per_wld_unit;
                m_obj_space_aabb =  math::Rect(-width / 2.0f, height / 2.0f, width, height);
        }

        // TODO: We need to handle the case of the get_tlas function returning a null pointer
        // in this case, we probably should have a default texture to use, an 'error texture'.
        // Another idea would be to use the color of the vertices to render something different
        // to alert about the missing texture atlas.
        Widget::vertex_data Text::get_view_space_vertices() const
        {
                if (m_characters_ids.empty()) {
                        return std::make_pair(nullptr, 0);
                }

                math::vec4  bottom_left_pos(m_rect.min());
                math::vec4  bottom_right_pos(bottom_left_pos.x + m_rect.width,
                                             bottom_left_pos.y);
                math::vec4  top_right_pos(m_rect.max());
                math::vec4  top_left_pos(m_rect.x, m_rect.y);

                const math::mat4 & view_space = m_pparent_canvas->get_transform_component()
                                                                  .get_object_to_world();

                // Transform the vertices 
                bottom_left_pos *= view_space;
                bottom_right_pos *= view_space;
                top_right_pos *= view_space;
                top_left_pos *= view_space;

                math::vec3 char_bottom_left(bottom_left_pos.x, bottom_left_pos.y, 0.0f);
                const float pixels_per_wld_unit = gfx::g_graphics_mgr.get_pixels_per_wld_unit();
                const int vertex_sz = 6;
                std::size_t i = 0;

                const float space_char_sz = (m_space_character_sz / pixels_per_wld_unit * m_scale_factor);
                const float space_between_chars = (m_characters_space / pixels_per_wld_unit * m_scale_factor);

                const gfx::Sprite_atlas * atlas = m_pparent_canvas->get_atlas();
                gfx::Vertex1P1C1UV *pbuffer = ui::g_vertex_buffer;
                for (; i != m_characters_ids.size(); ++i) {
                        const gfx::Atlas_image & image = atlas->get_image_data(m_characters_ids[i]);
                        float char_width_wld_units = (image.m_width * m_scale_factor) / pixels_per_wld_unit;
                        float char_height_wld_units = (image.m_height * m_scale_factor) / pixels_per_wld_unit;
                        const math::Rect & tcoordinates = image.m_texture_coordinates;

                        // check if it is a a space character
                        if (image.m_width == 0 && image.m_height == 0) {
                                char_bottom_left.x += space_char_sz;
                                continue;
                        }

                        // check if the new character fits on the text's rect
                        if (char_bottom_left.x + char_width_wld_units > bottom_right_pos.x) {
                                break;
                        }

                        const std::size_t start_index = vertex_sz * i;
                        pbuffer[start_index].m_pos = char_bottom_left;
                        pbuffer[start_index].m_uv.x = tcoordinates.x;
                        pbuffer[start_index].m_uv.y = tcoordinates.y + tcoordinates.height;

                        pbuffer[start_index + 1].m_pos.x = char_bottom_left.x + char_width_wld_units;
                        pbuffer[start_index + 1].m_pos.y = char_bottom_left.y;
                        pbuffer[start_index + 1].m_pos.z = 0.0f;
                        pbuffer[start_index + 1].m_uv.x = tcoordinates.x + tcoordinates.width;
                        pbuffer[start_index + 1].m_uv.y = tcoordinates.y + tcoordinates.height;

                        pbuffer[start_index + 2].m_pos.x = char_bottom_left.x + char_width_wld_units;
                        pbuffer[start_index + 2].m_pos.y = char_bottom_left.y + char_height_wld_units;
                        pbuffer[start_index + 2].m_pos.z = 0.0f;
                        pbuffer[start_index + 2].m_uv.x = tcoordinates.x + tcoordinates.width;
                        pbuffer[start_index + 2].m_uv.y = tcoordinates.y;

                        pbuffer[start_index + 3].m_pos = pbuffer[start_index + 2].m_pos;
                        pbuffer[start_index + 3].m_uv  = pbuffer[start_index + 2].m_uv;

                        pbuffer[start_index + 4].m_pos.x = char_bottom_left.x;
                        pbuffer[start_index + 4].m_pos.y = char_bottom_left.y + char_height_wld_units;
                        pbuffer[start_index + 4].m_pos.z = 0.0f;
                        pbuffer[start_index + 4].m_uv.x = tcoordinates.x;
                        pbuffer[start_index + 4].m_uv.y = tcoordinates.y;

                        pbuffer[start_index + 5].m_pos = pbuffer[start_index].m_pos;
                        pbuffer[start_index + 5].m_uv = pbuffer[start_index].m_uv;

                        char_bottom_left.x += char_width_wld_units + space_between_chars;
                }

                return std::make_pair(pbuffer, vertex_sz * i);
        }

}