#include "Path.hpp"
#include <cstdlib>
#include <iostream>

Path::Path(const char *str, const Separator separator, size_t num_chars) : m_separator(separator), m_strlen(num_chars) ,m_str(nullptr)
{
	if (m_strlen == 0) {
		//count the string
		const char *p = str;
		while ( *p++ != '\0') {
			++m_strlen;
		}
		//take into account the '\0'
		++m_strlen;
	}

	// allocate space for the path
	m_str = (char*)std::malloc(m_strlen);
	
	char *pdest = m_str;
	char sep = get_separator();

	//check for error
	if (sep == '\0') {
		//invalid separator
		free(m_str);
		m_str = nullptr;
		m_strlen = 0;
	}
	else {
		//copy the string
		for (; *str != '\0'; ++pdest, ++str) {
			if (is_separator(*str)) {
				*pdest = sep;
			}
			else {
				*pdest = *str;
			}
		}
		//add the null terminator character
		*pdest = '\0';
	}
}

Path::Path(const char *str): m_str(nullptr), m_strlen(0) 
{
	const char *porigin = str;
	
	//calculate size
	while (*porigin++ != '\0') {
		++m_strlen;
	}
	++m_strlen;

	//get separator
	porigin = str;
	while (*porigin++ != '\0') {
		if (is_separator(*porigin)) {
			m_separator = get_separator(*porigin);
			break;
		}
	}

	if (m_separator != INVALID_SEPARATOR) {
		//allocate str
		m_str = (char*)std::malloc(m_strlen);

		porigin = str;
		char *pdest = m_str;
		//copy str
		while (*porigin != '\0') {
			if (is_separator(*porigin)) {
				//this is to prevent a string with different separators
				*pdest = get_separator();
			}
			else {
				*pdest = *porigin;
			}
			++pdest;
			++porigin;
		}
		*pdest = '\0';
	}

}

Path::Path(const Path & path) : m_separator(path.m_separator), m_strlen(path.m_strlen)
{
	//allocate space for the new path string
	m_str = (char*)malloc(m_strlen);
	
	//copy the string
	const char *porigin = path.m_str;
	char *pdest = m_str;
	while ( (*pdest++ = *porigin++) != '\0') {
		;
	}
}

Path::Path(Path && path) : m_separator(path.m_separator), m_strlen(path.m_strlen), m_str(path.m_str) 
{
	path.m_strlen = 0;
	path.m_str = nullptr;
}

Path::~Path() 
{
	free(m_str);
	m_str	 = nullptr;
	m_strlen = 0;
}

Path & Path::operator=(const Path & rhs) 
{
	if (m_str != rhs.m_str) { //accounts for self assignment
		// check if size is enough
		if (m_strlen < rhs.m_strlen) {
			free(m_str);
			m_str = (char*)std::malloc(rhs.m_strlen);
		}

		m_separator = rhs.m_separator;
		m_strlen    = rhs.m_strlen;

		//copy the string
		const char *porigin = rhs.m_str;
		char *pdest = m_str;
		while ((*pdest++ = *porigin++) != '\0') {
			;
		}	
	}
	return *this;
}

Path & Path::Path::operator=(Path && rhs) 
{
	if (m_str != rhs.m_str) { //accounts for self assignment
		//deallocate resources
		free(m_str);
		
		//move resources from rhs to this object
		m_separator =  rhs.m_separator;
		m_strlen    =  rhs.m_strlen;
		m_str		=  rhs.m_str;

		rhs.m_strlen = 0;
		rhs.m_str = nullptr;
	}

	return *this;
}

Path & Path::convert(const Separator separator) 
{
	if (m_separator == separator) { 
		return *this;
	}

	m_separator = separator;
	char sep_char = get_separator();

	for (char *p = m_str; *p != '\0'; ++p) {
		if (is_separator(*p)) {
			*p = sep_char;
		}
	}

	return *this;
}

void Path::get_filename(char *filename) const
{
	//get a pointer to the '\0' character 
	const char *pstr_end = m_str + (m_strlen - 1);
	char sep = get_separator();
	while (*pstr_end != sep) {
		--pstr_end;
	}
	++pstr_end;

	//copy filename
	while ((*filename++ = *pstr_end++) != '\0') {
		;
	}
}

void Path::get_file_extension(char *file_extension) const 
{
	//get a pointer to the '\0' character
	const char *pstr_end = m_str + (m_strlen - 1);
	while (*pstr_end != '.' && (!is_separator(*pstr_end)) ) {
		--pstr_end;
	}
	if (*pstr_end == '.') {
		++pstr_end;
		//copy file extension
		while ((*file_extension++ = *pstr_end++) != '\0') {
			;
		}
	}
}

void Path::get_directory(char *directory) const 
{
	//get a pointer to the '\0' character
	const char *pstr_end = m_str + (m_strlen - 1);
	while (*pstr_end != '.' && (!is_separator(*pstr_end))) {
		--pstr_end;
	}

	if (*pstr_end == '.') {
		while (!is_separator(*--pstr_end)) {
			;
		}
		//copy from the beggining of the string, up to pstr_end;
		const char *porigin = m_str;
		while (porigin != pstr_end) {
			*directory++ = *porigin++;
		}
		*directory = '\0';
	}
	else {
		//the path is the directory
		const char *porigin = m_str;
		while ((*directory++ = *porigin++) != '\0') {
			;
		}
	}
}

bool Path::is_separator(const char c)  const
{
	switch (c) {
	case '/':
		return true;
	case '\\':
		return true;
	//case ':':
	//	return true;
	default:
		return false;
	}
}

char Path::get_separator() const
{
	switch (m_separator) {
	case BACKSLASH:
		return '\\';
	case FORWARD_SLASH:
		return '/';
	//case COLON:
	//	return ':';
	default:
		return '\0';
	}
}

Path::Separator Path::get_separator(const char c) const 
{
	switch (c) {
	case '\\':
		return BACKSLASH;
	case '/':
		return FORWARD_SLASH;
	//case ':':
	//	return COLON;
	default:
		return INVALID_SEPARATOR;
	}
}

std::ostream & operator<<(std::ostream & os, const Path & path) 
{
	os << path.m_str << std::endl;
	os << "Separator: " << path.get_separator() << std::endl;
	return os;
}