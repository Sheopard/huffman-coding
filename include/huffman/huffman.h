#ifndef HUFFMAN_CODE_H_
#define HUFFMAN_CODE_H_

#include <unordered_map>
#include <queue>
#include <vector>
#include <string>
#include <memory>

#include <huffman/io/input.h>
#include <huffman/io/output.h>

class Huffman final {
	class Node final {
		uint8_t byte_;
		int freq_;
		std::shared_ptr<Node> left_;
		std::shared_ptr<Node> right_;
	public:
		Node(const uint8_t byte, const int freq, 
		     const std::shared_ptr<Node>& left, 
		     const std::shared_ptr<Node>& right) noexcept;

		uint8_t byte() const noexcept;
		int freq() const noexcept;
		std::shared_ptr<Node> left() const noexcept;
		std::shared_ptr<Node> right() const noexcept;
	};

	Input::Reader input_;
	Output::Writer output_;
	bool encode_;

	std::vector<uint8_t> raw_bytes_;
	std::vector<uint8_t> finished_bytes_;
	std::unordered_map<uint8_t, Output::BitWriter> tree_;
	std::unordered_map<uint8_t, int> tree_len_;

	uint64_t total_compressed_len_ = 0;
	uint64_t header_bytes_ = 0;
	uint64_t compressed_bytes_ = 0;
public:
	explicit Huffman(const std::string& in, 
			 const std::string& out, 
			 const bool encode);

	void reset(const std::string& in, 
		   const std::string& out, 
		   const bool encode);

	void encode();
	void decode();
private:
	void preorder_print(const std::shared_ptr<Node>& root, 
			    std::queue<uint8_t>& q);

	std::shared_ptr<Node> restore_tree(std::queue<uint8_t>& q);
	
	void build_code(const std::shared_ptr<Node>& root, 
			const Output::BitWriter& writer, 
			const int len);

	void encode_data();
	void decode_data();

	void write_header(const std::shared_ptr<Node>& root);
	void write_compressed_bytes();
	void read_header();
	void read_compressed_bytes();

	void clear();
};

#endif
