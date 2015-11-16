#include "packet_reader.hpp"
#include "packet_writer.hpp"

packet_reader::packet_reader() 
	: offset(0)
{ 
	this->data.clear();
}

packet_reader::packet_reader(unsigned char* data, std::size_t size) 
	: offset(0), data(data, data + size) 
{ 

}

packet_reader::packet_reader(packet_reader& packet)
	: offset(packet.offset), data(packet.data)
{

}

packet_reader::~packet_reader()
{
	this->data.clear();
}

unsigned __int8 packet_reader::read_byte()
{
	return this->read<unsigned __int8>();
}

unsigned __int16 packet_reader::read_word()
{
	return this->read<unsigned __int16>();
}

unsigned __int32 packet_reader::read_dword()
{
	return this->read<unsigned __int32>();
}

unsigned __int64 packet_reader::read_qword()
{
	return this->read<unsigned __int64>();
}

bool packet_reader::read_buffer(unsigned char* buffer, std::size_t length)
{
	if (!this->check_offset(length))
	{
		return false;
	}

	memcpy(buffer, this->data.data() + this->offset, length);
	this->offset += length;
	return true;
}

std::string packet_reader::read_string()
{
	unsigned __int16 length = this->read_word();

	if (length == 0 || !this->check_offset(length))
	{
		return std::string();
	}
		
	this->offset += length;
	return std::string(reinterpret_cast<char*>(this->data.data() + this->offset - length), length);
}

bool packet_reader::indent(std::size_t length)
{
	if (!this->check_offset(length))
	{
		return false;
	}
		
	this->offset += length;
	return true;
}

unsigned char* packet_reader::get_data()
{
	return this->data.data();
}

std::size_t packet_reader::get_size()
{
	return this->data.size();
}

std::size_t packet_reader::get_remaining_size()
{
	return (this->data.size() - this->offset);
}

std::vector<unsigned char>& packet_reader::get_vector()
{
	return this->data;	
}

bool packet_reader::check_offset(std::size_t size)
{
	return ((this->offset + size) <= static_cast<std::size_t>(this->data.size()));
}