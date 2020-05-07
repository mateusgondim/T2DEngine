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

	//double the amout of characters for the size of the buffer
	m_capacity = (m_strlen - 1) * 2 + 1;

	// allocate space for the path
	m_str = (char*)std::malloc(m_capacity);
	
	if (m_str == nullptr) {
		return;
	}

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

	//double the amout of characters for the size of the buffer
	m_capacity = (m_strlen - 1) * 2 + 1;

	//get separator
	porigin = str;
	while (*porigin != '\0') {
		if (is_separator(*porigin)) {
			m_separator = get_separator(*porigin);
			break;
		}
		++porigin;
	}

	if (m_separator != INVALID_SEPARATOR) {
		//allocate str
		m_str = (char*)std::malloc(m_capacity);
		
		if (m_str == nullptr) { // unable to allocate
			m_capacity = 0;
			m_strlen   = 0;
			std::cerr << __FUNCTION__ << ":Unable to allocate memory" << std::endl;
			return;
		}

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

Path::Path(const Path & path) : m_separator(path.m_separator), m_strlen(path.m_strlen), m_capacity(path.m_capacity)
{
	//allocate space for the new path string
	m_str = (char*)malloc(m_capacity);
	if (m_str != nullptr) {
		//copy the string
		const char *porigin = path.m_str;
		char *pdest = m_str;
		while ((*pdest++ = *porigin++) != '\0') {
			;
		}
	}
	else {
		std::cerr << __FUNCTION__ << ": Unable to allocate memory" << std::endl;
	}
}

Path::Path(Path && path) noexcept : m_separator(path.m_separator), m_strlen(path.m_strlen), m_capacity(path.m_capacity), m_str(path.m_str) 
{
	path.m_strlen = 0;
	path.m_capacity = 0;
	path.m_str = nullptr;
}

Path::~Path() 
{
	free(m_str);
	m_str = nullptr;
	m_strlen = 0;
	m_capacity = 0;
}

Path & Path::operator=(const Path & rhs) 
{
	if (m_str != rhs.m_str) { //accounts for self assignment
		// check if the capacity is big enough to hold the characters in rhs
		if (m_capacity < rhs.m_strlen) {
			char *pbuffer = (char*)std::malloc(rhs.m_capacity);
			if (pbuffer != nullptr) {
				free(m_str);
				m_str = pbuffer;
				m_capacity = rhs.m_capacity;
			}
			else {
				std::cerr << __FUNCTION__ << ": Unable to allocate memory" << std::endl;
				return *this;
			}
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

Path & Path::Path::operator=(Path && rhs) noexcept
{
	if (m_str != rhs.m_str) { //accounts for self assignment
		//deallocate resources
		free(m_str);
		
		//move resources from rhs to this object
		m_separator =  rhs.m_separator;
		m_strlen    =  rhs.m_strlen;
		m_capacity  =  rhs.m_capacity;
		m_str		=  rhs.m_str;

		rhs.m_strlen    = 0;
		rhs.m_capacity  = 0;
		rhs.m_str       = nullptr;
	}

	return *this;
}

Path & Path::operator+=(const Path & path) 
{
	size_t expanded_sz = (m_strlen + path.m_strlen) - 1; //accounts for two '\0' characters
	
	char *pdest;
	const char *porigin;

	//check if this Path object has a capacity big enough to hold  both character strings
	if (expanded_sz > m_capacity) { 
		//rellocate the characters
		expanded_sz = (expanded_sz - 1) * 2 + 1;
		char *pbuffer = (char*)malloc(expanded_sz);
		if (pbuffer != nullptr) {
			m_capacity = expanded_sz;

			//copy this m_str
			pdest = pbuffer;
			porigin = m_str;
			while ((*pdest++ = *porigin++) != '\0') {
				;
			}
			//free the old m_str buffer and assign the new one to it 
			free(m_str);
			m_str = pbuffer;
		}
		else {
			std::cerr << __FUNCTION__ << ": Unable to allocate memory" << std::endl;
			return *this;
		}
	}

	//copy the second string
	pdest = m_str + (m_strlen - 1);
	porigin = path.m_str;
	char sepchar = get_separator();
	while (*porigin != '\0') {
		if (is_separator(*porigin)) {
			*pdest = sepchar;
		}
		else {
			*pdest = *porigin;
		}
		++porigin;
		++pdest;
	}
	*pdest = '\0';

	m_strlen += (path.m_strlen - 1);

	return *this;
}

Path & Path::operator+=(const char *str) 
{
	//check for nullptr
	if (str == nullptr) {
		return *this;
	}

	//calculate the size of the string
	size_t sz = 0;
	for (const char *straux = str; *straux != '\0'; ++straux) {
		++sz;
	}

	size_t expanded_sz = m_strlen + sz; 
	char *pdest;
	const char *porigin;
	
	if (expanded_sz > m_capacity) {
		//rellocate the character string
		expanded_sz = (expanded_sz - 1) * 2 + 1;
		char *pbuffer = (char*)malloc(expanded_sz);
		if (pbuffer != nullptr) {
			m_capacity = expanded_sz;

			//copy this m_str
			pdest = pbuffer;
			porigin = m_str;
			while ((*pdest++ = *porigin++) != '\0') {
				;
			}
			//free the old m_str buffer and assign the new one to it 
			free(m_str);
			m_str = pbuffer;
		}
		else {
			std::cerr << __FUNCTION__ << ": Unable to allocate memory" << std::endl;
		}
	}
	
	//copy the second string
	pdest = m_str + (m_strlen - 1);
	porigin = str;
	char sepchar = get_separator();
	while (*porigin != '\0') {
		if (is_separator(*porigin)) {
			*pdest = sepchar;
		}
		else {
			*pdest = *porigin;
		}
		++porigin;
		++pdest;
	}
	*pdest = '\0';
	m_strlen += sz;

	return *this;
}

Path operator+(const Path & lhs, const Path & rhs) 
{
	Path r = lhs;
	r += rhs;

	return r;
}

Path operator+(const Path & lhs, const char *rhs) 
{
	Path r = lhs;
	r += rhs;

	return r;
}

Path operator+(const char *lhs, const Path & rhs) 
{
	//check for nullptr
	if (lhs == nullptr) {
		return rhs;
	}

	//calculate the size of lhs
	size_t len = 0;
	for (const char *straux = lhs; *straux != '\0'; ++straux) {
		len++;
	}
	len++;

	//get the separator character from lhs
	Path::Separator separator = Path::INVALID_SEPARATOR;
	const char *straux = lhs;
	while (*straux != '\0') {
		if (rhs.is_separator(*straux)) {
			separator = rhs.get_separator(*straux);
			break;
		}
		++straux;
	}

	//badly formed path string
	if (separator == Path::INVALID_SEPARATOR) {
		return rhs;
	}

	Path r(lhs, separator, len);
	r += rhs;

	return r;
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

void Path::reserve(const size_t n) 
{
	if ((m_str == nullptr) || (n < m_capacity) ) {
		return;
	}

	char *pbuffer = (char*)malloc(n);
	if (pbuffer == nullptr) {
		return;
	}

	m_capacity = n;

	//copy the characters
	char *pdest = pbuffer;
	const char *porigin = m_str;
	while ((*pdest++ = *porigin++) != '\0') {
		;
	}

	//free the old buffer
	free(m_str);
	m_str = pbuffer;
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