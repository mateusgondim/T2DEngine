#ifndef _TEXT_HPP
#define _TEXT_HPP

#include "Widget.hpp"

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace ui { class Canvas; }
namespace math { struct Rect; }

namespace ui
{
        class Text final : public Widget {
                friend class Canvas;
        public:
                Text(Canvas & parent_canvas, const math::Rect & rect, const std::string & msg,
                     const std::size_t obj_sz, const float scale_factor = 1.0f,
                     const int characters_space = 1, const int space_character_sz = 6);
                Text(const Text &) = delete;
                Text & operator=(const Text &) = delete;

                const std::string &     get_message() const { return m_msg; }
                float get_scale_factor() const { return m_scale_factor; }
        protected:
                virtual vertex_data     get_view_space_vertices() const override;
                const float m_scale_factor;
                const int   m_characters_space; // the space in pixels between two caracters on the text
                const int   m_space_character_sz; // the size in pixels of the 'space' character
                std::vector<std::uint32_t> m_characters_ids;
                std::string m_msg;
        };
}

#endif // !_TEXT_HPP
