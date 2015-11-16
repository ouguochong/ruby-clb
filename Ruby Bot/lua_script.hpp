#pragma once

extern "C" 
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include <luabind\luabind.hpp>
#include <luabind\adopt_policy.hpp>

#include "stage_base.hpp"
#include "packet_reader.hpp"

#include <string>

class lua_script
{
public:
	lua_script();
	~lua_script();

	void update_stage(maplestory::stage::stage_base* stage);
	void load_script(const std::string& script_file);
	
	bool send_packet(packet_writer& packet);

	void update_table_string(const std::string& key, const std::string& value);
	void update_table_int32(const std::string& key, unsigned __int32 value);
	void update_table_int64(const std::string& key, unsigned __int64 value, bool delimited = false);

	void handle_on_map_id(unsigned int map_id);
	void handle_on_packet(unsigned int header, packet_reader& packet);

	void set_map_id_handler(unsigned int map_id, luabind::object object);
	void set_packet_handler(unsigned int header, luabind::object object);

private:
	std::size_t get_handler_index_by_map_id(unsigned int map_id);
	std::size_t get_handler_index_by_packet_header(unsigned int header);

	maplestory::stage::stage_base* stage;

	lua_State* L;

	std::vector<std::pair<unsigned int, luabind::object>> map_id_handlers;
	std::vector<std::pair<unsigned int, luabind::object>> packet_handlers;
};