#include "packet_writer.hpp"
#include "packet_reader.hpp"

#include <iterator>

packet_writer::packet_writer()
{

}

packet_writer::packet_writer(unsigned __int16 header)
{ 
	this->write_word(header);
}

packet_writer::packet_writer(packet_writer& packet)
	: data(packet.data)
{

}

packet_writer::~packet_writer()
{
	this->data.clear();
}

void packet_writer::write_byte(unsigned __int8 byte)
{
	this->write<unsigned __int8>(byte);
}

void packet_writer::write_word(unsigned __int16 word)
{
	this->write<unsigned __int16>(word);
}

void packet_writer::write_dword(unsigned __int32 dword)
{
	this->write<unsigned __int32>(dword);
}
	
void packet_writer::write_qword(unsigned __int64 qword)
{
	this->write<unsigned __int64>(qword);
}

void packet_writer::write_buffer(unsigned char* buffer, std::size_t length)
{
	std::copy(buffer, buffer + length, std::back_inserter(this->data));
}

void packet_writer::write_string(std::string const& string)
{
	this->write_word(static_cast<unsigned __int16>(string.length()));
	std::copy(string.begin(), string.end(), std::back_inserter(this->data));
}
	
void packet_writer::write_zeroes(std::size_t length)
{
	for (std::size_t i = 0; i < length; i++)
	{
		this->data.push_back(0x00);
	}
}	

std::size_t packet_writer::get_size()
{
	return this->data.size();
}

unsigned char* packet_writer::get_data()
{
	return this->data.data();
}

std::vector<unsigned char>& packet_writer::get_vector()
{
	return this->data;	
}