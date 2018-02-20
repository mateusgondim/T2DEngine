#include "test.hpp"
#include "Pool_allocator.hpp"
#include <iostream>

int main(int argc, char *argv[]) 
{
	std::cout << sizeof(void*) << std::endl;
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


	return 0;
}