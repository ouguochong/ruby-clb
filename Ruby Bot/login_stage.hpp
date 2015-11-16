#pragma once

#include "stage_base.hpp"

#include "maple_client.hpp"

#include "account.hpp"

#include "character_info.hpp"
#include "world_info.hpp"

namespace maplestory
{
	namespace stage
	{
		class login_stage : public stage_base
		{
		public:
			login_stage(std::size_t index, maple_client* client, account& account);
			~login_stage();
			
		protected:
			bool on_enter_stage();
		
			bool handle_handshake(unsigned char locale, unsigned short version_major, unsigned short version_minor);
			bool handle_packet(unsigned short header, packet_reader& packet);
	
		private:
			bool send_select_world();
			bool send_select_channel();
			bool send_select_character();

			bool on_login_map();
			bool on_world_information(packet_reader& packet);
			bool on_select_world_result(packet_reader& packet);
			bool on_check_guest_id_login_result(packet_reader& packet);
			bool on_select_channel_result(packet_reader& packet);
			bool on_select_character_result(packet_reader& packet);

		private:
			maple_client* client;

			account account;
			bool succesfully_logged_in;

			std::vector<world_info> worlds;
			std::vector<character_info> characters;
		};
	}
}