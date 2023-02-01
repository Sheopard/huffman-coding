#include <huffman/io/input.h>

Input::Reader::Reader(const std::string& filename) {
	in.open(filename, std::ios_base::binary);
}

Input::Reader::~Reader() {
	this->close();
}

uint8_t Input::Reader::byte() {
	uint8_t byte = in.get();
	return byte;
}

uint16_t Input::Reader::word() {
	uint16_t word = (in.get() << 8) | in.get();
	return word;
}

uint32_t Input::Reader::dword() {
	uint32_t dword = (((uint32_t)in.get())) | (((uint32_t)in.get()) << 8) | (((uint32_t)in.get()) << 16) | (((uint32_t)in.get()) << 24);
	return dword;
}

uint64_t Input::Reader::qword() {
	uint64_t qword = (((uint64_t)in.get())) | (((uint64_t)in.get()) << 8) | (((uint64_t)in.get()) << 16) | (((uint64_t)in.get()) << 24) | (((uint64_t)in.get()) << 32) | (((uint64_t)in.get()) << 40) | (((uint64_t)in.get()) << 48) | (((uint64_t)in.get()) << 56);
	return qword;
}

void Input::Reader::rewind() {
	in.seekg(0);
}

bool Input::Reader::eof() {
	return !in;
}

void Input::Reader::reopen(const std::string& filename) {
	this->close();
	in.open(filename, std::ios_base::binary);
}

void Input::Reader::close() {
	if (in.is_open()) {
		in.close();
	}
}

Input::BitReader::BitReader(const std::vector<uint8_t>& bytes) : next_byte_(0), next_bit_(0) {
	std::copy(bytes.begin(), bytes.end(), std::back_inserter(bytes_));
}

uint8_t Input::BitReader::bit() {
	uint8_t bit = (bytes_[next_byte_] >> (7 - next_bit_)) & 0b1;

	next_bit_++;
	if (next_bit_ > 7) {
		next_byte_++;
		next_bit_ = 0;
	}

	return bit;
}

void Input::BitReader::clear() {
	bytes_.clear();
	next_byte_ = 0;
	next_bit_ = 0;
}
