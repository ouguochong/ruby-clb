#include "lua_script.hpp"

#include "packet_reader.hpp"
#include "packet_writer.hpp"

#include "main_form.hpp"
#include "utility.hpp"

namespace general
{
	static unsigned int random(unsigned int max)
	{
		if (max == 0)
		{
			return static_cast<unsigned int>(rand());
		}
		else
		{
			return static_cast<unsigned int>(rand() % max);
		}
	}
}

class printer
{
public:
	printer()
	{
	
	}
	
	void print(int y)
	{
		printf("%d\n\n", y);
	}

	void printhex(int y)
	{
		printf("%08X\n\n", y);
	}

	void printdouble(double d)
	{
		printf("%f\n\n", d);
	}

	void printstring(std::string s)
	{
		printf("%s\n\n", s.c_str());
	}

	void dump_packet(packet_writer& pkt)
	{
		this->hexdump(pkt.get_data(), pkt.get_size());			
	}

private:
	void hexdump(void* input, int length)
	{
		unsigned char* buffer = reinterpret_cast<unsigned char*>(input);

		for (int i = 0; i < length; i += 16)
		{
			printf("%06X: ", i);

			for (int j = 0; j < 16; j++)
			{
				if (i + j < length)
				{
					printf("%02X ", buffer[i + j]);
				}
				else
				{
					printf("   ");
				}
			}

			printf(" ");

			for (int j = 0; j < 16; j++)
			{
				if (i + j < length)
				{
					printf("%c", isprint(buffer[i + j]) ? buffer[i + j] : '.');
				}
			}

			printf("\n");
		}
	}
};

lua_script::lua_script() 
	: L(luaL_newstate())
{
	luabind::open(this->L);
	luabind::module(this->L)
	[
		luabind::def("random", general::random),

		luabind::class_<printer>("printer")
			.def(luabind::constructor<>())
			.def("print", &printer::print)
			.def("printhex", &printer::printhex)
			.def("printdouble", &printer::printdouble)
			.def("printstring", &printer::printstring)
			.def("dump_packet", &printer::dump_packet),
			
		luabind::class_<packet_reader>("packet_reader")
			.def(luabind::constructor<unsigned char*, std::size_t>())
			.def(luabind::constructor<packet_reader&>())
			.def("read_byte", &packet_reader::read_byte)
			.def("read_word", &packet_reader::read_word)
			.def("read_dword", &packet_reader::read_dword)
			.def("read_qword", &packet_reader::read_qword)
			.def("read_buffer", &packet_reader::read_buffer)
			.def("read_string", &packet_reader::read_string)
			.def("indent", &packet_reader::indent)
			.def("get_size", &packet_reader::get_size)
			.def("get_data", &packet_reader::get_data),

		luabind::class_<packet_writer>("packet_writer")
			.def(luabind::constructor<>())
			.def(luabind::constructor<unsigned __int16>())
			.def(luabind::constructor<packet_writer&>())
			.def("write_byte", &packet_writer::write_byte)
			.def("write_word", &packet_writer::write_word)
			.def("write_dword", &packet_writer::write_dword)
			.def("write_qword", &packet_writer::write_qword)
			.def("write_buffer", &packet_writer::write_buffer)
			.def("write_string", &packet_writer::write_string)
			.def("write_zeroes", &packet_writer::write_zeroes)
			.def("get_size", &packet_writer::get_size)
			.def("get_data", &packet_writer::get_data),

		luabind::class_<lua_script>("lua_script")
			.def("send_packet", &lua_script::send_packet)
			.def("update_table_string", &lua_script::update_table_string)
			.def("update_table_int32", &lua_script::update_table_int32)
			.def("update_table_int64", &lua_script::update_table_int64)
			.def("set_map_id_handler", &lua_script::set_map_id_handler)
			.def("set_packet_handler", &lua_script::set_packet_handler)
	];
}

lua_script::~lua_script()
{
	lua_close(this->L);
}

void lua_script::update_stage(maplestory::stage::stage_base* stage)
{
	this->stage = stage;
}

void lua_script::load_script(const std::string& script)
{
	luaL_dofile(this->L, ("scripts\\" + script).c_str());

	try
	{
		luabind::call_function<void>(this->L, "main", boost::ref(*this));
	}
	catch (const std::exception& error)
	{
		printf("[load_script] lua error: %s\n", error.what());
	}
}

bool lua_script::send_packet(packet_writer& packet)
{
	return this->stage->send_packet(packet);
}

void lua_script::update_table_string(const std::string& key, const std::string& value)
{
	gui::main_form::get_instance().update_table(this->stage->get_index(), key, value);
}

void lua_script::update_table_int32(const std::string& key, unsigned __int32 value)
{
	gui::main_form::get_instance().update_table(this->stage->get_index(), key, value);
}

void lua_script::update_table_int64(const std::string& key, unsigned __int64 value, bool delimited)
{
	gui::main_form::get_instance().update_table(this->stage->get_index(), key, value);
}

void lua_script::handle_on_map_id(unsigned int map_id)
{
	std::size_t index = this->get_handler_index_by_map_id(map_id);

	if (index != static_cast<std::size_t>(-1))
	{
		luabind::call_function<void>(this->map_id_handlers.at(index).second, boost::ref(*this));
	}
}

void lua_script::handle_on_packet(unsigned int header, packet_reader& packet)
{
	std::size_t index = this->get_handler_index_by_packet_header(header);

	if (index != static_cast<std::size_t>(-1))
	{
		luabind::call_function<void>(this->packet_handlers.at(index).second, boost::ref(*this), boost::ref(packet));
	}
}

void lua_script::set_map_id_handler(unsigned int map_id, luabind::object object)
{
	try
	{
		if (luabind::type(object) == LUA_TFUNCTION)
		{
			if (this->get_handler_index_by_map_id(map_id) == static_cast<std::size_t>(-1))
			{
				this->map_id_handlers.push_back(std::make_pair(map_id, object));
			}
			else
			{
				printf("[set_map_id_handler] attempted to add a handler for an already handled map id.\n");
			}
		}
		else
		{
			printf("[set_map_id_handler] handler must be a function of type std::function<void(lua_script&)>.\n");
		}
	}
	catch (const std::exception& error)
	{
		printf("[set_map_id_handler] lua error: %s.\n", error.what());
	}
}

void lua_script::set_packet_handler(unsigned int header, luabind::object object)
{
	try
	{
		if (luabind::type(object) == LUA_TFUNCTION)
		{
			if (this->get_handler_index_by_packet_header(header) == static_cast<std::size_t>(-1))
			{
				this->packet_handlers.push_back(std::make_pair(header, object));
			}
			else
			{
				printf("[set_packet_handler] attempted to add a handler for an already handled header.\n");
			}
		}
		else
		{
			printf("[set_packet_handler] handler must be a function of type std::function<void(lua_script&, packet_reader&)>.\n");
		}
	}
	catch (const std::exception& error)
	{
		printf("[set_packet_handler] lua error: %s\n", error.what());
	}
}

std::size_t lua_script::get_handler_index_by_map_id(unsigned int map_id)
{
	for (std::size_t i = 0; i < this->map_id_handlers.size(); i++)
	{
		if (this->map_id_handlers.at(i).first == map_id)
		{
			return i;
		}
	}

	return static_cast<std::size_t>(-1);
}

std::size_t lua_script::get_handler_index_by_packet_header(unsigned int header)
{
	for (std::size_t i = 0; i < this->packet_handlers.size(); i++)
	{
		if (this->packet_handlers.at(i).first == header)
		{
			return i;
		}
	}

	return static_cast<std::size_t>(-1);
}