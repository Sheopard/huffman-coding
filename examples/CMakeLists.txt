cmake_minimum_required(VERSION 3.8)

project(examples-huffman-coding VERSION 0.0.1 LANGUAGES CXX)

add_executable(example.exe example.cc)

set_target_properties(example.exe PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
)

target_link_libraries(example.exe huffman-lib)
