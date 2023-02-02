#include <iostream>
#include <huffman/huffman.h>

Huffman::Node::Node(const uint8_t byte, 
		    const int freq, 
		    const std::shared_ptr<Node>& left, 
		    const std::shared_ptr<Node>& right) noexcept
		    			: byte_(byte), 
		    			freq_(freq),
		    			left_(left),
		    			right_(right) {}

uint8_t Huffman::Node::byte() const noexcept {
	return byte_;
}

int Huffman::Node::freq() const noexcept {
	return freq_;
}

std::shared_ptr<Huffman::Node> Huffman::Node::left() const noexcept {
	return left_;
}

std::shared_ptr<Huffman::Node> Huffman::Node::right() const noexcept {
	return right_;
}

Huffman::Huffman(const std::string& in, 
		 const std::string& out, 
		 const bool encode)
		 	   : input_(in),
	       		   output_(out),
	       		   encode_(encode) {

	if (encode_) {
		while(!input_.eof()) {
			raw_bytes_.push_back(input_.byte());
		}
		raw_bytes_.pop_back();
	} else {
		total_compressed_len_ = input_.qword();
		header_bytes_ = input_.qword();
		compressed_bytes_ = input_.qword();

		read_header();
		read_compressed_bytes();
	}
}

void Huffman::preorder_print(const std::shared_ptr<Node>& root,
	       		     std::queue<uint8_t>& q) {
	if (root == nullptr) {
		return;
	}

	if (!root->left() && !root->right()) {
		q.push(0b1);
		q.push(root->byte());
	} else {
		q.push(0b0);
	}
	preorder_print(root->right(), q);
	preorder_print(root->left(), q);
}

std::shared_ptr<Huffman::Node> Huffman::restore_tree(std::queue<uint8_t>& q) {
	if (q.empty()) {
		return nullptr;
	}

	int val = q.front();
	q.pop();
	
	if (val == 0b1) {
		val = q.front();
		q.pop();
		return std::make_shared<Node>(val, 0, nullptr, nullptr);
	} else {
		std::shared_ptr<Node> right = restore_tree(q);
		std::shared_ptr<Node> left = restore_tree(q);
		return std::make_shared<Node>(0, 0, left, right);
	}
}

void Huffman::encode_data() {
	Output::BitWriter writer;
	for (size_t i = 0, size = raw_bytes_.size(); i < size; i++) {
		auto tree_res = tree_.find(raw_bytes_[i]);
		auto tree_len_res = tree_len_.find(raw_bytes_[i]);

		if (tree_res != tree_.end() && tree_len_res != tree_len_.end()) {
			int len = tree_len_res->second;
			total_compressed_len_ += len;
			auto byte_to_write = tree_res->second.get();

			int cur_len = 0;
			for (int k = 0; k < byte_to_write.size(); k++) {
				if (len > 7) {
					cur_len = 8;
					len -= 8;
				} else {
					cur_len = len;
					len = 0;
				}

				for (int j = 0; j < cur_len; j++) {
					writer.put_bit(
						(byte_to_write[k] >> (7 - j)) & 0b1
					);
				}
			}
		}
	}

	finished_bytes_ = writer.get();
	compressed_bytes_ = finished_bytes_.size();
}

void Huffman::build_code(const std::shared_ptr<Node>& root, 
			 const Output::BitWriter& writer, 
			 const int len) {

	if (root == nullptr) {
		return;
	}

	if (!root->left() && !root->right()) {
		tree_[root->byte()] = writer;
		tree_len_[root->byte()] = len;
	}

	build_code(root->left(), writer + 0, len + 1);
	build_code(root->right(), writer + 1, len + 1);
}


void Huffman::encode() {
	if (!encode_) {
		throw std::runtime_error("Error: Encoding attempt in decoding mode");
	}

	std::unordered_map<uint8_t, int> frequences;
	for (auto i : raw_bytes_) {
		frequences[i]++;
	}

	auto cmp = [](const std::shared_ptr<Node>& left, 
		      const std::shared_ptr<Node>& right) { 
		      return left->freq() > right->freq();
	};

	std::priority_queue<std::shared_ptr<Node>, 
			    std::vector<std::shared_ptr<Node>>, 
			    decltype(cmp)> heap(cmp);

	for (auto i : frequences) {
		heap.push(std::make_shared<Node>(i.first, i.second, nullptr, nullptr));
	}

	std::shared_ptr<Node> first = nullptr;
	std::shared_ptr<Node> second = nullptr;
	while(heap.size() != 1) {
		first = heap.top();
		heap.pop();
		second = heap.top();
		heap.pop();

		heap.push(
			std::make_shared<Node>(0, 
				first->freq() + second->freq(), 
				first, 
				second)
		);
	}

	const std::shared_ptr<Node>& root = heap.top();

	Output::BitWriter bw;
	build_code(root, bw, 0);
	encode_data();

	write_header(root);
	write_compressed_bytes();
}

void Huffman::decode_data() {
	Input::BitReader br(raw_bytes_);
	Output::BitWriter bw;

	int len = 0;
	int cur_len = 0;
	int encoded_size = 0;
	uint8_t bit = 0;
	std::vector<uint8_t> encoded_vector;
	std::vector<uint8_t> cur_code;
	int j = 0;

	while(total_compressed_len_--) {
		bit = br.bit();
		len++;

		bw.put_bit(bit);
		encoded_vector = bw.get();
		encoded_size = encoded_vector.size();

		for (auto i : tree_) {
			auto find_res = tree_len_.find(i.first);
			if (find_res == tree_len_.end()) {
				continue;
			}

			cur_len = find_res->second;
			cur_code = i.second.get();
			if (cur_code.size() == encoded_size && len == cur_len) {
				j = 0;
				while (j < encoded_size && cur_code[j] == encoded_vector[j]) {
					j++;
				}

				if (j == encoded_size) {
					output_.put_byte(i.first);
					bw.clear();
					len = 0;
				}
			}
		}
	}
}

void Huffman::decode() {
	if (encode_) {
		throw std::runtime_error("Error: Decoding attempt in encoding mode");
	}

	decode_data();
}

void Huffman::write_header(const std::shared_ptr<Huffman::Node>& root) {
	if (root == nullptr) {
		return;
	}

	std::queue<uint8_t> q;
	preorder_print(root, q); 
	
	header_bytes_ = q.size();

	output_.put_qword(total_compressed_len_);
	output_.put_qword(header_bytes_);
	output_.put_qword(compressed_bytes_);

	while(!q.empty()) {
		output_.put_byte(q.front());
		q.pop();
	}
}

void Huffman::write_compressed_bytes() {
	for (int i = 0; i < compressed_bytes_; i++) {
		output_.put_byte(finished_bytes_[i]);
	}
}

void Huffman::read_header() {
	std::queue<uint8_t> q;
	while(header_bytes_--) {
		q.push(input_.byte());
	}

	const std::shared_ptr<Node>& root = restore_tree(q);
	preorder_print(root, q);

	Output::BitWriter bw;
	build_code(root, bw, 0);
}

void Huffman::read_compressed_bytes() {
	for (int i = 0; i < compressed_bytes_; i++) {
		raw_bytes_.push_back(input_.byte());
	}
}

void Huffman::clear() {
	raw_bytes_.clear();
	finished_bytes_.clear();
	tree_.clear();
	tree_len_.clear();

	total_compressed_len_ = 0;
	header_bytes_ = 0;
	compressed_bytes_ = 0;
}

void Huffman::reset(const std::string& in, 
		    const std::string& out, 
		    const bool encode) {

	clear();

	input_.reopen(in);
	output_.reopen(out);
	encode_ = encode;

	if (encode_) {
		while(!input_.eof()) {
			raw_bytes_.push_back(input_.byte());
		}
		raw_bytes_.pop_back();
	} else {
		total_compressed_len_ = input_.qword();
		header_bytes_ = input_.qword();
		compressed_bytes_ = input_.qword();

		read_header();
		read_compressed_bytes();
	}
}
