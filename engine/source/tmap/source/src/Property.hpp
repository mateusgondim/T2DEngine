#ifndef _PROPERTY_HPP
#define _PROPERTY_HPP

#include <string>

class Property final {
friend bool operator<(const Property & lhs, const Property & rhs);
public:
	enum Type { STRING_PROPERTY, FLOAT_PROPERTY, INT_PROPERTY, BOOL_PROPERTY, FILE_PROPERTY, COLOR_PROPERTY, INVALID_PROPERTY };
	Property(const char *pname, const Type type);
        Property(const std::string & element);
	Property(const Property & p);
	Property(Property && p) noexcept;
	Property();

	void		set_value(const char *str);
	void		set_value(const float fvalue);
	void		set_value(const int ivalue);
	void		set_value(const bool bvalue);
	
	Type		get_type() const;
	const char *	get_name() const;

	const  char *	get_str()    const;
	float		get_float()  const;
	int		get_int()    const;
	bool		get_bool()   const;

	~Property();

private:
	char		*m_pname;
	Type		m_type;   
	union {
		char		*m_strvalue;
		float		m_fvalue;
		int		m_ivalue;
		bool		m_bvalue;
	};
};

bool operator<(const Property & lhs, const Property & rhs);
std::ostream & operator<<(std::ostream & os, const Property & p);

inline const char *Property::get_str() const 
{
	if (m_type == Type::STRING_PROPERTY || m_type == Type::FILE_PROPERTY) {
		return m_strvalue;
	}
	return nullptr;
}

inline float Property::get_float() const 
{
	if (m_type == Type::FLOAT_PROPERTY) {
		return m_fvalue;
	}
	return -1;
}

inline int Property::get_int() const 
{
	if (m_type == Type::INT_PROPERTY) {
		return m_ivalue;
	}
	return -1;
}

inline bool Property::get_bool() const 
{
	if (m_type == Type::BOOL_PROPERTY) {
		return m_bvalue;
	}

	return false;
}

inline Property::Type Property::get_type() const 
{
	return m_type;
}

inline const char *Property::get_name() const 
{
	return m_pname;
}

#endif // !_PROPERTY_HPP
