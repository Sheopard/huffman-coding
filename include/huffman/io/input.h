#ifndef HUFFMAN_INPUT_H_
#define HUFFMAN_INPUT_H_

#include <fstream>
#include <vector>

namespace Input {
class Reader final {
	std::ifstream in;
public:
	Reader() = default;
	explicit Reader(const std::string& filename);
	~Reader();

	uint8_t byte();
	uint16_t word();
	uint32_t dword();
	uint64_t qword();

	void rewind();
	bool eof();

	void reopen(const std::string& filename);
	void close();
};

class BitReader final {
	std::vector<uint8_t> bytes_;
	uint64_t next_byte_;
	uint8_t next_bit_;
public:
	BitReader() = delete;
	explicit BitReader(const std::vector<uint8_t>& bytes);
	uint8_t bit();

	void clear();
};

} // namespace Input

#endif
