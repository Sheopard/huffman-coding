#include <huffman/io/output.h>

Output::Writer::Writer(const std::string& filename) {
	out.open(filename, std::ios_base::binary);
}

Output::Writer::~Writer() {
	this->close();
}

void Output::Writer::put_byte(uint8_t byte) {
	out.put(byte);	
}

void Output::Writer::put_word(uint16_t word) {
	out.put(word & 0xff);
	out.put((word >> 8) & 0xff);
}

void Output::Writer::put_dword(uint32_t dword) {
	out.put(dword & 0xff);
	out.put((dword >> 8) & 0xff);
	out.put((dword >> 16) & 0xff);
	out.put((dword >> 24) & 0xff);
}

void Output::Writer::put_qword(uint64_t qword) {
	out.put(qword & 0xff);
	out.put((qword >> 8) & 0xff);
	out.put((qword >> 16) & 0xff);
	out.put((qword >> 24) & 0xff);
	out.put((qword >> 32) & 0xff);
	out.put((qword >> 40) & 0xff);
	out.put((qword >> 48) & 0xff);
	out.put((qword >> 56) & 0xff);
}

void Output::Writer::reopen(const std::string& filename) {
	this->close();
	out.open(filename, std::ios_base::binary);
}

void Output::Writer::close() {
	if (out.is_open()) {
		out.close();
	}
}

Output::BitWriter::BitWriter() : next_byte_(0), next_bit_(0) {}

Output::BitWriter::BitWriter(const Output::BitWriter& rhs) {
	next_byte_ = rhs.next_byte_;
	next_bit_ = rhs.next_bit_;

	std::copy(rhs.bytes_.begin(), rhs.bytes_.end(), std::back_inserter(bytes_));
}

void Output::BitWriter::put_bit(uint8_t bit) {
	if (bytes_.empty()) {
		bytes_.push_back(0);	
	}

	if (bit == 0b1) {
		bytes_[next_byte_] |= (bit << (7 - next_bit_));
	}

	next_bit_++;
	if (next_bit_ > 7) {
		bytes_.push_back(0);
		next_byte_++;
		next_bit_ = 0;
	}
}

void Output::BitWriter::clear() {
	bytes_.clear();
	next_byte_ = 0;
	next_bit_ = 0;
}

Output::BitWriter& Output::BitWriter::operator+=(const uint8_t bit) {
	put_bit(bit);	
	return *this;
}

Output::BitWriter Output::operator+(const Output::BitWriter& lhs, const uint8_t bit) {
	BitWriter result(lhs);
	result += bit;
	return result;
}
