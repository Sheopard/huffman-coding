cmake_minimum_required(VERSION 3.8)

project(huffman-coding VERSION 0.0.1 LANGUAGES CXX)

set(Headers
	include/huffman/io/input.h
	include/huffman/io/output.h
	include/huffman/huffman.h
)

set(Sources
	src/huffman/io/input.cc
	src/huffman/io/output.cc
	src/huffman/huffman.cc
)

add_library(huffman-lib STATIC ${Sources} ${Headers})

set_target_properties(huffman-lib PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
)

target_include_directories(huffman-lib PUBLIC include)

add_subdirectory(examples)
