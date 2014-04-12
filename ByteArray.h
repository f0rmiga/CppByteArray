//============================================================================
// Name        : ByteArray.h
// Author      : Thulio Ferraz Assis
// Contact     : contato@thulioassis.com.br
// Copyright   : Thulio Ferraz Assis all rights reserved
//============================================================================

#include <vector>

using namespace std;

#define LITTLE_ENDIAN_ 0
#define BIG_ENDIAN_ 1

class ByteArray {
public:
	/* Initialize empty ByteArray with predefined size */
	ByteArray(unsigned int size_) {
		size = size_; // Set the private var "size" to the provided "size_"
		bytesAvailable_ = size; // Set the bytes available
		vector_.reserve(size); // Reserve the size in memory
	}
	/* Initialize ByteArray using existing vector of char */
	ByteArray(vector<char> newvector_) {
		size = newvector_.size(); // Set the private var "size" to the size of the provided vector<char>
		bytesAvailable_ = size; // Set the bytes available
		vector_.reserve(size); // Reserve the size in memory
		vector_.assign(newvector_.begin(), newvector_.end()); // Assign to the private vector<char> used by the class the provided "newvector_"
	}
	/* Initialize empty ByteArray with predefined size and endianness option */
	ByteArray(unsigned int size_, unsigned char endianness_) {
		size = size_;
		bytesAvailable_ = size;
		vector_.reserve(size);
		if (endianness_ == BIG_ENDIAN_ || endianness_ == LITTLE_ENDIAN_)
			endianness = endianness_;
	}
	/* Initialize ByteArray using existing vector of char and endianness option */
	ByteArray(vector<char> newvector_, unsigned char endianness_) {
		size = newvector_.size();
		bytesAvailable_ = size;
		vector_.reserve(size);
		vector_.assign(newvector_.begin(), newvector_.end());
		if (endianness_ == BIG_ENDIAN_ || endianness_ == LITTLE_ENDIAN_)
			endianness = endianness_;
	}

	/* Set the position to write or read */
	void setPosition(unsigned int position_) {
		position = position_; // set the private var "position" to the provided "position_"
		bytesAvailable_ = size - position; // set the bytes available based on the size and the new position
	}

	/* Returns actual position to write or read */
	unsigned int getPosition() {
		return position;
	}

	/* Returns the number of bytes available to read */
	unsigned int bytesAvailable() {
		return bytesAvailable_;
	}

	/* Returns related to the vector */
	vector<char> result() { return vector_; } // Returns the vector<char> of the ByteArray to use in other classes or libraries
	unsigned int getSize() { return size; }

	/* Write functions */
	/* returns true if wrote, otherwise, returns false */
	bool writeByte(char byte_) {
		if (position > size - 1) { // Verifies if the ByteArray has available space to write 1 byte
			return false; // Returns false if it doesn't
		}
		vector_[position] = byte_; // Writes the byte in the current position
		position++; // Moves the position
		bytesAvailable_ = size - position; // Calculate the new bytes available
		return true;
	}

	bool writeShort(short short_) {
		if (bytesAvailable_ < 2) { // Verifies if the ByteArray has available space to write 2 bytes
			return false; // Returns false if it doesn't
		}
		if (endianness == LITTLE_ENDIAN_)
			return writeByte(short_ & 0xFF) && writeByte((short_ >> 8) & 0xFF); // Writes the little endian sequence
		else
			return writeByte((short_ >> 8) & 0xFF) && writeByte(short_ & 0xFF); // Writes the big endian sequence
	}

	bool writeInt(int int_) {
		if (bytesAvailable_ < 4) { // Verifies if the ByteArray has available space to write 4 bytes
			return false; // Returns false if it doesn't
		}
		if (endianness == LITTLE_ENDIAN_)
			return writeByte(int_ & 0xFF) && writeByte((int_ >> 8) & 0xFF) && writeByte((int_ >> 16) & 0xFF) && writeByte((int_ >> 24) & 0xFF); // Writes the little endian sequence
		else
			return writeByte((int_ >> 24) & 0xFF) && writeByte((int_ >> 16) & 0xFF) && writeByte((int_ >> 8) & 0xFF) && writeByte(int_ & 0xFF); // Writes the big endian sequence
	}

	bool writeUTF(string string_) {
		if (bytesAvailable_ < string_.size()) { // Verifies if the ByteArray has available space to write the entire string
			return false; // Returns false if it doesn't
		}
		for (unsigned int i = 0; i < string_.size(); i++) { // Loop thru the string to write
			writeByte(string_[i]); // Writes each char to the ByteArray
		}
		return true;
	}

	/* Read functions */
	/* Throws exception if try to read out of bounds */
	char readByte() {
		if (position >= size) { // Stop the read if it has no bytes available to read
			throw 1; // Throws error
		}
		char byte_ = vector_[position]; // Gets the byte in the current position
		position++; // Moves the position
		bytesAvailable_ = size - position; // Calculate the new bytes available
		return byte_; // Returns the byte
	}
	char readByte(unsigned int offset) {
		if (offset < size) { // Sets the position to the provided offset only if the read will handle it
			position = offset;
		}
		return readByte();
	}

	unsigned char readUnsignedByte() {
		return readByte();
	}
	unsigned char readUnsignedByte(unsigned int offset) {
		return readByte(offset);
	}

	short readShort() {
		if (position >= size -1) { // Stop the read if it has not enough bytes available to read
			throw 1; // Throws error
		}
		short short_ = ((int)(unsigned char)vector_[position + (endianness == LITTLE_ENDIAN_ ? 1 : 0)] << 8) | (unsigned char)vector_[position + (endianness == LITTLE_ENDIAN_ ? 0 : 1)]; // Gets the bytes in the right sequence based on the endianness
		position += 2; // Moves the position by 2 bytes
		bytesAvailable_ = size - position; // Calculate the new bytes available
		return short_; // Returns the short
	}
	short readShort(unsigned int offset) {
		if (offset < size - 1) { // Sets the position to the provided offset only if the read will handle it
			position = offset;
		}
		return readShort();
	}

	unsigned short readUnsignedShort() {
		return readShort();
	}
	unsigned short readUnsignedShort(unsigned int offset) {
		return readShort(offset);
	}

	int readInt() {
		if (position >= size -3) { // Stop the read if it has not enough bytes available to read
			throw 1; // Throws error
		}
		int int_ = ((int)(unsigned char)vector_[position + (endianness == LITTLE_ENDIAN_ ? 3 : 0)] << 24) | ((int)(unsigned char)vector_[position + (endianness == LITTLE_ENDIAN_ ? 2 : 1)] << 16) | ((int)(unsigned char)vector_[position + (endianness == LITTLE_ENDIAN_ ? 1 : 2)] << 8) | (unsigned char)vector_[position + (endianness == LITTLE_ENDIAN_ ? 0 : 3)]; // Gets the bytes in the right sequence based on the endianness
		position += 4; // Moves the position by 4 bytes
		bytesAvailable_ = size - position; // Calculate the new bytes available
		return int_; // Returns the int
	}
	int readInt(unsigned int offset) {
		if (offset < size - 3) { // Sets the position to the provided offset only if the read will handle it
			position = offset;
		}
		return readInt();
	}

	unsigned int readUnsignedInt() {
		return readInt();
	}
	unsigned int readUnsignedInt(unsigned int offset) {
		return readInt(offset);
	}

	string readUTF(unsigned int size_) {
		if (size_ > size - position) { // Stop the read if the size provided exceeds the bytes available to read
			throw 1; // Throws error
		}
		string string_; // Instantiate new string to return
		for (unsigned int o = 0; o < size_; position++, o++) { // Loop thru the ByteArray to push to the string
			string_.push_back(vector_[position]); // Pushes each byte to the string
		}
		bytesAvailable_ = size - position; // Calculate the new bytes available
		return string_; // Returns the string
	}
	string readUTF(unsigned int offset, unsigned int size_) {
		if (size_ <= size - offset) { // Sets the position to the provided offset only if the read will handle it
			position = offset;
		}
		return readUTF(size_);
	}

private:
	/* The vector of chars */
	vector<char> vector_;

	/* Position of the offset in read and write operations */
	unsigned int position = 0;

	/* Number of bytes available to read */
	unsigned int bytesAvailable_;

	/* Fixed size of the ByteArray */
	unsigned int size = 0;

	/* Define the endianness, little endian by default*/
	unsigned char endianness = LITTLE_ENDIAN_;
};