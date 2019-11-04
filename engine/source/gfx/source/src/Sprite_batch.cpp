#include "Sprite_batch.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cstdint>

#include "Vertex1P1C1UV.hpp"
#include "Sprite.hpp"

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"


gfx::Sprite_batch::Sprite_batch(const unsigned max_num_vertices, const bool is_static) :
	m_max_num_vertices(max_num_vertices), m_num_used_vertices(0), m_vao(0), m_vbo(0), m_is_static(is_static)
{
	std::cout << __FUNCTION__ << " max_num_vertices = " << max_num_vertices << " | Buffer size = " << sizeof(Vertex1P1C1UV) * max_num_vertices << " bytes" << std::endl;

	glGetError();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex1P1C1UV) * max_num_vertices, 0, GL_DYNAMIC_DRAW);

	//vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex1P1C1UV), 0);
	glEnableVertexAttribArray(0);

	//vertex color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex1P1C1UV), (GLvoid*)offsetof(Vertex1P1C1UV, m_col));
	glEnableVertexAttribArray(1);

	//vertex uv coordinate
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex1P1C1UV), (GLvoid*)offsetof(Vertex1P1C1UV, m_uv));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	if (GL_NO_ERROR != glGetError()) {
		std::cerr << __FUNCTION__ << " failed to create batch" << std::endl;
	}

}

void gfx::Sprite_batch::reset()
{
        m_num_used_vertices = 0;
}



void gfx::Sprite_batch::add(const gfx::Vertex1P1C1UV * pvertex_buffer, const std::size_t buffer_sz)
{
        if ((m_max_num_vertices - m_num_used_vertices) < buffer_sz) {
                std::cout << __FUNCTION__ << " there is not enough room for " << buffer_sz 
                          << " vertices in this batch. This batch already have " 
                          << m_num_used_vertices << " vertices and can only store " 
                          << m_max_num_vertices << std::endl;
        }
        else {
                glBindVertexArray(m_vao);
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

                glBufferSubData(GL_ARRAY_BUFFER, m_num_used_vertices * sizeof(Vertex1P1C1UV),
                                buffer_sz * sizeof(Vertex1P1C1UV), pvertex_buffer);

                glBindVertexArray(0);

                m_num_used_vertices += buffer_sz;
        }
}

//add(): add vertices to the buffer 
void gfx::Sprite_batch::add(const std::vector<gfx::Vertex1P1C1UV> & vertices) 
{
	if ((m_max_num_vertices - m_num_used_vertices) < vertices.size() ) {
		std::cout << __FUNCTION__ << " there is not enough room for " << vertices.size() << " vertices in this batch. This batch already have " 
			      << m_num_used_vertices << " vertices and can only store "    << m_max_num_vertices << std::endl;
	}
	else {
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glBufferSubData(GL_ARRAY_BUFFER, m_num_used_vertices * sizeof(Vertex1P1C1UV), vertices.size() * sizeof(Vertex1P1C1UV), &vertices[0]);

		glBindVertexArray(0);

		m_num_used_vertices += vertices.size();
	}
	
}

void gfx::Sprite_batch::add(const gfx::Sprite  *psprite)
{
	//assumes vertex_position has the same size as vertex_uv!!
	const math::vec3  *position			=  psprite->get_vertex_position_vec();
	const math::vec2  *texture_coord	=  psprite->get_vertex_uv_vec();

	const std::size_t vec_sz = VERTICES_PER_SPRITE;

	if ((m_max_num_vertices - m_num_used_vertices) < vec_sz) {
		std::cout << __FUNCTION__ << " there is not enough room for " << vec_sz << " vertices in this batch. This batch already have "
			<< m_num_used_vertices << " vertices and can only store " << m_max_num_vertices << std::endl;
	}
	else {
		//std::vector<Vertex1P1C1UV> vertices; //change this!! it involvers dynamic memory allocation, maybe add a data member to Sprite_batch
		for (int i = 0; i < vec_sz; ++i) {
			//Vertex1P1C1UV vertex(position[i], math::vec4(), texture_coord[i]);
			//vertices.push_back(vertex);
            m_sprite_aux_buffer[i].m_pos = position[i];
            m_sprite_aux_buffer[i].m_col = math::vec4();
            m_sprite_aux_buffer[i].m_uv = texture_coord[i];
		}
		//transfer data to vbo
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glBufferSubData(GL_ARRAY_BUFFER, m_num_used_vertices * sizeof(Vertex1P1C1UV), vec_sz * sizeof(Vertex1P1C1UV), &m_sprite_aux_buffer[0]);

		glBindVertexArray(0);

		m_num_used_vertices += vec_sz;

	}
}
//maybe we pass the set of texture here...
void gfx::Sprite_batch::render()
{
	glBindVertexArray(m_vao);
	
	//m_texture.use();

	glDrawArrays(GL_TRIANGLES, 0, m_num_used_vertices);
	glBindVertexArray(0);

	//reset buffer if it is dynamic
	if (!m_is_static) {
		m_num_used_vertices = 0;
	}
}


gfx::Sprite_batch::~Sprite_batch() 
{
	if (glIsBuffer(m_vbo)) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
		if (glIsVertexArray(m_vao)) {
			glDeleteVertexArrays(1, &m_vao);
			m_vao = 0;
		}
	}
}