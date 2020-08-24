#ifndef _PROPERTY_HPP
#define _PROPERTY_HPP

#include <cstddef>

#include <string>

#include "string_id.hpp"

class Property final {
friend bool operator<(const Property & lhs, const Property & rhs);
public:
    enum Type { STRING, FLOAT, INT, BOOL, FILE, COLOR, INVALID };
	Property(const char *pname, const Type type);
    explicit Property(const std::string & element);
	Property(const Property & p);
	Property(Property && p) noexcept;
	~Property();

	void set_value(const char *str);
	void set_value(const float fvalue);
	void set_value(const int ivalue);
	void set_value(const bool bvalue);
	
	Type		get_type() const;
	const char *	get_name() const;

	const  char *	get_str()    const;
	float		get_float()  const;
	int		get_int()    const;
	bool		get_bool()   const;

	~Property();
    string_id get_name_id() const;

private:
	char *m_pname;
    string_id m_name_id;
	Type m_type;   
    union {
        char *m_strvalue;
        float m_fvalue;
		int m_ivalue;
		bool m_bvalue;
	};
};

bool operator<(const Property & lhs, const Property & rhs);
std::ostream & operator<<(std::ostream & os, const Property & p);



inline const char *Property::get_str() const 
{
	if (m_type == Type::STRING || m_type == Type::FILE) {
		return m_strvalue;
	}
	return nullptr;
}

inline float Property::get_float() const 
{
	if (m_type == Type::FLOAT) {
		return m_fvalue;
	}
	return -1;
}

inline int Property::get_int() const 
{
	if (m_type == Type::INT) {
		return m_ivalue;
	}
	return -1;
}

inline bool Property::get_bool() const 
{
	if (m_type == Type::BOOL) {
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

inline string_id Property::get_name_id() const
{
    return m_name_id;
}
#endif // !_PROPERTY_HPP
