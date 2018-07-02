#ifndef _ANIMATOR_CONTROLLER_PARAMETER_HPP
#define _ANIMATOR_CONTROLLER_PARAMETER_HPP

#include "string_id.hpp"

/* This class defines a Animator_controller parameter.
 * A parameter can be one of four possible types, those are: integer, floating point
 * boolean and trigger. Since it can only be of one type at any moment, we store a union
 * data memember that have all possible types as its members. To know what is the type of 
 * the parameter, we assign to a enum data member, a constant value indicating the type.
 */

namespace gfx {
	class Animator_controller_parameter final {
	public:
		enum class Type {FLOAT, INT, BOOL, TRIGGER};
		Animator_controller_parameter();
		Animator_controller_parameter(const string_id name_id, const Type type);

		Animator_controller_parameter(const Animator_controller_parameter & parameter);
		Animator_controller_parameter & operator=(const Animator_controller_parameter & rhs);

		~Animator_controller_parameter() {}

		string_id	get_id()   const { return m_name_id; }
		Type        get_type() const { return m_type; }

		float	get_float() const;
		int		get_int() const;
		bool    get_bool() const;

		void set_value(const int   ivalue);
		void set_value(const float fvalue);
		void set_value(const bool  bvalue);
	private:
		string_id	m_name_id;
		Type		m_type;
		union {
			float	m_default_float;
			int		m_default_int;
			bool	m_default_bool;
		};

	};
}

#endif // !_ANIMATOR_CONTROLLER_PARAMETER_HPP
