# Huffman Coding Algorithm Library

This library compresses the entire file with Huffman code. 

## Usage

To work with the library, you need to specify the encoded file, as well as the output file.
You can do this by calling the constructor:

```cpp
Huffman::Huffman(const std::string& in, const std::string& out, const bool encode);
```

After calling the constructor, you can use the encode function:

```cpp
void Huffman::encode();
```

If you want to decode, you can use the decode function:

```cpp
void Huffman::decode();
```

If you want to reset files and mode, you can use the reset function:

```cpp
void Huffman::reset(const std::string& in, const std::string& out, const bool encode);
```
