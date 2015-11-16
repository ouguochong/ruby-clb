#pragma once

#include <vector>
#include <string>

class packet_reader
{
public:
	packet_reader();
	packet_reader(unsigned char* data, std::size_t size);
	packet_reader(packet_reader& packet);
	virtual ~packet_reader();

	unsigned __int8 read_byte();
	unsigned __int16 read_word();
	unsigned __int32 read_dword();
	unsigned __int64 read_qword();

	bool read_buffer(unsigned char* buffer, std::size_t length);
	std::string read_string();
	
	template<typename T>
	T read_string_as();

	bool indent(std::size_t length);
	
	unsigned char* get_data();
	std::size_t get_size();
	std::size_t get_remaining_size();
	
	std::vector<unsigned char>& get_vector();

private:
	template <typename T>
	T read();

	bool check_offset(std::size_t size);

	std::size_t offset;
	std::vector<unsigned char> data;
};

template <typename T>
T packet_reader::read()
{
	if (this->check_offset(sizeof(T)))
	{
		this->offset += sizeof(T);
		return *reinterpret_cast<T*>(this->data.data() + this->offset - sizeof(T));
	}

	return static_cast<T>(0);
}

template<typename T>
T packet_reader::read_string_as()
{
	unsigned short length = this->read<unsigned short>();

	if (this->check_offset(length))
	{
		std::string buffer(reinterpret_cast<char*>(this->data.data() + this->offset), length);
		this->offset += length;

		return static_cast<T>(atoi(buffer.c_str()));
	}

	return static_cast<T>(0);
}