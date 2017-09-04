#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Sprite_batch.hpp"
#include "Shader.hpp"
#include "Texture_2d.hpp"
#include "vec4.hpp"

tgs::Sprite_batch::Sprite_batch(const unsigned max_num_vertices, const tgs::Texture_2d & texture, const gls::Shader & shader, const bool is_static) :
	m_max_num_vertices(max_num_vertices), m_num_used_vertices(0), 
	m_texture(texture), m_vao(0), m_vbo(0), m_is_static(is_static)
{
	std::cout << __FUNCTION__ << " max_num_vertices = " << max_num_vertices << " | Buffer size = " << sizeof(Vertex1P1C1UV) * max_num_vertices << " bytes" << std::endl;

	glGetError();

	shader.use();

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


//add(): add vertices to the buffer 
void tgs::Sprite_batch::add(const std::vector<tgs::Vertex1P1C1UV> & vertices) 
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

void tgs::Sprite_batch::add(const tgs::Sprite & sprite) 
{
	//assumes vertex_position has the same size as vertex_uv!!
	const std::vector<cgm::vec3> & position			=  sprite.get_vertex_position_vec();
	const std::vector<cgm::vec2> & texture_coord	=  sprite.get_vertex_uv_vec();

	if ((m_max_num_vertices - m_num_used_vertices) < position.size()) {
		std::cout << __FUNCTION__ << " there is not enough room for " << position.size() << " vertices in this batch. This batch already have "
			<< m_num_used_vertices << " vertices and can only store " << m_max_num_vertices << std::endl;
	}
	else {
		std::vector<Vertex1P1C1UV> vertices;
		for (int i = 0; i < position.size(); ++i) {
			Vertex1P1C1UV vertex(position[i], cgm::vec4(), texture_coord[i]);
			vertices.push_back(vertex);
		}
		//transfer data to vbo
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glBufferSubData(GL_ARRAY_BUFFER, m_num_used_vertices * sizeof(Vertex1P1C1UV), vertices.size() * sizeof(Vertex1P1C1UV), &vertices[0]);

		glBindVertexArray(0);

		m_num_used_vertices += vertices.size();

	}
}

void tgs::Sprite_batch::render() 
{
	glBindVertexArray(m_vao);
	
	m_texture.use();

	glDrawArrays(GL_TRIANGLES, 0, m_num_used_vertices);
	glBindVertexArray(0);

	//reset buffer if it is dynamic
	if (!m_is_static) {
		m_num_used_vertices = 0;
	}
}