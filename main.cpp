#include <iostream>
#include <huffman/huffman.h>

int main(int argc, char* argv[]) {
	if (argc < 3) {
		return -1;
	}

	Huffman huff(argv[1], argv[2], true);
	try {
		huff.encode();
	} catch (const std::runtime_error& exception) {
		std::cout << exception.what() << "\n";
	}

	huff.reset(argv[2], argv[3], false);
	try {
		huff.decode();
	} catch (const std::runtime_error& exception) {
		std::cout << exception.what() << "\n";
	}

	return 0;
}
