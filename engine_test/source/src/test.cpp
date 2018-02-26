#include "test.hpp"
#include "Pool_allocator.hpp"
#include <string>
#include "crc32.hpp"
#include <iostream>

int main(int argc, char *argv[]) 
{
	/*
	Pool_allocator pool(4, 5, 4);

	void *chunk_01, *chunk_02, *chunk_03, *chunk_04, *chunk_05;
	
	std::cout << pool << std::endl;
	chunk_01 = pool.get_element();
	std::cout << pool << std::endl;
	chunk_02 = pool.get_element();
	std::cout << pool << std::endl;
	chunk_03 = pool.get_element();
	std::cout << pool << std::endl;
	chunk_04 = pool.get_element();
	std::cout << pool << std::endl;
	chunk_05 = pool.get_element();
	std::cout << pool << std::endl;

	void *extra_chunk;

	extra_chunk = pool.get_element();

	pool.free_element(chunk_02);
	std::cout << pool << std::endl;
	pool.free_element(chunk_04);
	std::cout << pool << std::endl;
	chunk_02 = pool.get_element();
	std::cout << pool << std::endl;
	pool.free_element(chunk_01);
	std::cout << pool << std::endl;
	pool.free_element(chunk_02);
	std::cout << pool << std::endl;
	pool.free_element(chunk_05);
	std::cout << pool << std::endl;
	pool.free_element(chunk_03);
	std::cout << pool << std::endl;
	*/

	std::string word_01 = "player";
	std::string word_02 = "enemy01";
	std::string word_03 = "enemy02";
	std::string word_04 = "sword";
	std::string word_05 = "knife";
	std::string word_06 = "enemy03";
	std::string word_07 = "car";
	std::string word_08 = "zombi";
	std::string word_09 = "bomb";
	std::string word_10 = "fire";

	std::cout << "checksum for " << word_01 << ": " << std::hex << get_crc32(word_01.c_str()) << std::endl;
	std::cout << "checksum for " << word_02 << ": " << std::hex << get_crc32(word_02.c_str()) << std::endl;
	std::cout << "checksum for " << word_03 << ": " << std::hex << get_crc32(word_03.c_str()) << std::endl;
	std::cout << "checksum for " << word_04 << ": " << std::hex << get_crc32(word_04.c_str()) << std::endl;
	std::cout << "checksum for " << word_05 << ": " << std::hex << get_crc32(word_05.c_str()) << std::endl;
	std::cout << "checksum for " << word_06 << ": " << std::hex << get_crc32(word_06.c_str()) << std::endl;
	std::cout << "checksum for " << word_07 << ": " << std::hex << get_crc32(word_07.c_str()) << std::endl;
	std::cout << "checksum for " << word_08 << ": " << std::hex << get_crc32(word_08.c_str()) << std::endl;
	std::cout << "checksum for " << word_09 << ": " << std::hex << get_crc32(word_09.c_str()) << std::endl;
	std::cout << "checksum for " << word_10 << ": " << std::hex << get_crc32(word_10.c_str()) << std::endl;

	return 0;
}