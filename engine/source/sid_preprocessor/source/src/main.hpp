#ifndef _SID_PREPROCESSOR_HPP
#define _SID_PREPROCESSOR_HPP

#include <string>
#include <fstream>

void preprocess_line(std::string  line, std::fstream & output_file_stream);
void sid_parser(const char * unprocessed_file_path, const char * preprocessed_file_path);

#endif // !_SID_PREPROCESSOR_HPP
