#include "mat4.hpp"
#include "vec3.hpp"

cgm::mat4::mat4() 
{
	m_elements[0][0] = 1.0f;	m_elements[0][1] = 0.0f;	m_elements[0][2] = 0.0f;	m_elements[0][3] = 0.0f;
	m_elements[1][0] = 0.0f;	m_elements[1][1] = 1.0f;	m_elements[1][2] = 0.0f;	m_elements[1][3] = 0.0f;
	m_elements[2][0] = 0.0f;	m_elements[2][1] = 0.0f;	m_elements[2][2] = 1.0f;	m_elements[2][3] = 0.0f;
	m_elements[3][0] = 0.0f;	m_elements[3][1] = 0.0f;	m_elements[3][2] = 0.0f;	m_elements[3][3] = 1.0f;
}

cgm::mat4::mat4(const cgm::vec3 &  p, const cgm::vec3 & q, const cgm::vec3 & r, const cgm::vec3 & t) 
{
	m_elements[0][0] = p.x; m_elements[0][1] = p.y; m_elements[0][2] = p.z; m_elements[0][3] = 0.0f;
	m_elements[1][0] = q.x; m_elements[1][1] = q.y; m_elements[1][2] = q.z; m_elements[1][3] = 0.0f;
	m_elements[2][0] = r.x; m_elements[2][1] = r.y; m_elements[2][2] = r.z; m_elements[2][3] = 0.0f;
	m_elements[3][0] = t.x; m_elements[3][1] = t.y; m_elements[3][2] = t.z; m_elements[3][3] = 1.0f;
}

cgm::mat4::mat4(const cgm::mat4 & L, const cgm::vec3 & t) 
{
	m_elements[0][0] = L.m_elements[0][0];	m_elements[0][1] = L.m_elements[0][1];	m_elements[0][2] = L.m_elements[0][2];	m_elements[0][3] = L.m_elements[0][3];
	m_elements[1][0] = L.m_elements[1][0];	m_elements[1][1] = L.m_elements[1][1];	m_elements[1][2] = L.m_elements[1][2];	m_elements[1][3] = L.m_elements[1][3];
	m_elements[2][0] = L.m_elements[2][0];	m_elements[2][1] = L.m_elements[2][1];	m_elements[2][2] = L.m_elements[2][2];	m_elements[2][3] = L.m_elements[2][3];
	m_elements[3][0] = t.x;					m_elements[3][1] = t.y;					m_elements[3][2] = t.z;					m_elements[3][3] = 1.0f;
}

cgm::mat4 & cgm::mat4::transpose() 
{
	float aux;

	aux = m_elements[1][0];
	m_elements[1][0] = m_elements[0][1];
	m_elements[0][1] = aux;

	aux = m_elements[2][0];
	m_elements[2][0] = m_elements[0][2];
	m_elements[0][2] = aux;

	aux = m_elements[2][1];
	m_elements[2][1] = m_elements[1][2];
	m_elements[1][2] = aux;

	aux = m_elements[3][0];
	m_elements[3][0] = m_elements[0][3];
	m_elements[0][3] = aux;

	aux = m_elements[3][1];
	m_elements[3][1] = m_elements[1][3];
	m_elements[1][3] = aux;

	aux = m_elements[3][2];
	m_elements[3][2] = m_elements[2][3];
	m_elements[2][3] = aux;

	return *this;
	
}