#pragma once

#include <vector>
#include <string>

class packet_writer
{
public:
	packet_writer();
	packet_writer(unsigned __int16 header);
	packet_writer(packet_writer& packet);
	virtual ~packet_writer();

	void write_byte(unsigned __int8 byte);
	void write_word(unsigned __int16 word);
	void write_dword(unsigned __int32 dword);
	void write_qword(unsigned __int64 qword);

	void write_buffer(unsigned char* buffer, std::size_t length);
	void write_string(std::string const& string);
	
	void write_zeroes(std::size_t length);
		
	std::size_t get_size();
	unsigned char* get_data();

	std::vector<unsigned char>& get_vector();

private:
	template <typename T>
	void write(T data);

	std::vector<unsigned char> data;
};

template <typename T>
void packet_writer::write(T data)
{
	unsigned char* temp_data = reinterpret_cast<unsigned char*>(&data);
	std::copy(temp_data, temp_data + sizeof(T), std::back_inserter(this->data));
}