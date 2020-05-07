#ifndef _PATH_HPP

#define _PATH_HPP

/* Utility class to represent file paths, it can convertert
 * beetwheen different file path separators, extract the filename,
 * file directory and file extension.
 */
#include <iostream>

class Path final {
friend std::ostream & operator<<(std::ostream & os, const Path & path);
friend Path operator+(const char *lhs, const Path & rhs);
public:
	enum Separator {FORWARD_SLASH, BACKSLASH, INVALID_SEPARATOR};
		  Path(const char *str, const Separator separator, size_t num_chars = 0);
		  Path(const char *str);

		  Path(const Path & path);
		  Path(Path && path) noexcept;

		  Path & operator=(const Path & path);
		  Path & operator=(Path && path) noexcept;

		  ~Path();

		  Path & operator+=(const Path & path);
		  Path & operator+=(const char *str);

	      Path & convert(const Separator separator);
	void  get_filename(char *filename) const;
	void  get_file_extension(char *file_extesion) const;
	void  get_directory(char *directory) const;
	const char *c_str() const { return m_str; }

	size_t	get_length()   const { return m_strlen - 1; }
	size_t  get_capacity() const { return m_capacity; }
	void    reserve(const size_t n);
private:
	bool is_separator(const char c) const;
	char get_separator() const;
	Separator get_separator(const char c) const;
	
	Separator m_separator;
	char	  *m_str;
	size_t	  m_strlen;
	size_t    m_capacity; 
};

Path operator+(const Path & lhs, const Path & rhs);
Path operator+(const Path & lhs, const char *rhs);
Path operator+(const char *lhs, const Path & rhs);

std::ostream & operator<<(std::ostream & os, const Path & path);

#endif // !_PATH_HPP
