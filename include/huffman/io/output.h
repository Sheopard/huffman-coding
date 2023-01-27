#ifndef HUFFMAN_OUTPUT_H_
#define HUFFMAN_OUTPUT_H_

#include <fstream>
#include <vector>

namespace Output {

class Writer final {
	std::ofstream out;
public:
	Writer() = default;
	explicit Writer(const std::string& filename);
	~Writer();
	
	void put_byte(uint8_t byte);
	void put_word(uint16_t word);
	void put_dword(uint32_t dword);
	void put_qword(uint64_t qword);

	void reopen(const std::string& filename);
	void close();
};

class BitWriter final {
	std::vector<uint8_t> bytes_;
	uint64_t next_byte_;
	uint8_t next_bit_;
public:
	BitWriter();
	BitWriter(const BitWriter& rhs);
	void put_bit(uint8_t bit);
	void clear();

	std::vector<uint8_t> get() const { return bytes_; };
	BitWriter& operator+=(const uint8_t bit);
};

BitWriter operator+(const BitWriter& lhs, const uint8_t bit);

} // namespace Output

#endif
