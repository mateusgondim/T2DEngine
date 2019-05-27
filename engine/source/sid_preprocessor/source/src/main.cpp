#include "main.hpp"
#include "crc32.hpp"
#include <iomanip>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstddef>

/**
 * Parse a line of code, replacing the single quoted string 
 * in the macro SID('any-string'), by the corresponding hashed
 * integer value, writing the parsed line on a output file
*/
void preprocess_line(std::string  line, std::fstream & output_file_stream)
{
        const std::string keyword("SID");
        const std::string blank_characters(" \t\r");
        const std::string alphanum("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM0123456789_");
        std::size_t sid_pos = 0;
        sid_pos = line.find(keyword, sid_pos);
        while (sid_pos != std::string::npos) {
                auto opening_paren_pos = line.find_first_not_of(blank_characters,
                                                                sid_pos + keyword.length());
                if (opening_paren_pos == std::string::npos || line[opening_paren_pos] != '(') {
                        std::cerr << "Error: could not find open param" << std::endl;
                        sid_pos = line.find(keyword, opening_paren_pos);
                        continue;
                }
                auto first_quote_pos = line.find_first_not_of(blank_characters,
                                                              opening_paren_pos + 1);
                if (first_quote_pos == std::string::npos || line[first_quote_pos] != '\'') {
                        sid_pos = first_quote_pos;
                        std::cerr << "Error: could not find opening quote" << std::endl;
                        sid_pos = line.find(keyword, first_quote_pos);
                        continue;
                }

                auto second_quote_pos = line.find_first_not_of(alphanum, first_quote_pos + 1);
                if (second_quote_pos == std::string::npos || line[second_quote_pos] != '\'' || 
                    second_quote_pos == first_quote_pos + 1) {
                        std::cerr << "ERROR: Missing closing quote" << std::endl;
                        sid_pos = line.find(keyword, second_quote_pos);
                        continue;
                }
                auto closing_paren_pos = line.find_first_not_of(blank_characters,
                                                                second_quote_pos + 1);
                // check if closing paren was found
                if (closing_paren_pos == std::string::npos || line[closing_paren_pos] != ')') {
                        std::cerr << "ERROR: Missing closing paren" << std::endl;
                        sid_pos = line.find(keyword, closing_paren_pos);
                        continue;
                }

                // if everthing is ok, get the string and apply the hash function
                std::string str_to_hash = line.substr(first_quote_pos + 1,
                                                      second_quote_pos - (first_quote_pos + 1));
                std::uint32_t hashed_str = get_crc32(str_to_hash.c_str());

                std::stringstream hex_str;
                hex_str << "0x" << std::setfill('0') << std::setw(sizeof(std::uint32_t) * 2) 
                        << std::hex << hashed_str;
                
                line.replace(opening_paren_pos + 1, closing_paren_pos - opening_paren_pos - 1,
                             hex_str.str());
                // recalculate the position of closing paren before searching for the next word
                closing_paren_pos = line.find_first_of(')', opening_paren_pos);
                sid_pos = line.find(keyword, closing_paren_pos + 1);
        }

        // write to output file
        output_file_stream << line << '\n';
}

/**
 * Get a file stream to both an input and an output
 * source file and, iterate through the input file 
 * running the sid parser on each line
 */

void sid_parser(const char * unprocessed_file_path, const char * preprocessed_file_path)
{
        std::fstream preprocessed_stream(preprocessed_file_path,
                                         std::fstream::in | std::fstream::out | std::fstream::trunc);
        std::ifstream unprocessed_file(unprocessed_file_path);
        if (!preprocessed_stream.good()) {
                // file does not exist create it first
                std::ofstream outfile_stream(preprocessed_file_path);
                outfile_stream.close();
                preprocessed_stream.close();
                preprocessed_stream.open(preprocessed_file_path,
                                         std::fstream::in | std::fstream::out);
        }
        std::string line;
        while (std::getline(unprocessed_file, line)) {
                preprocess_line(line, preprocessed_stream);
        }

}

int main(int argc, char * argv[])
{
        if (argc < 2) {
                return -1;
        }

        const char *raw_file_path = argv[1];
        const char *preprocessed_file_path = argv[2];
        sid_parser(raw_file_path, preprocessed_file_path);
}