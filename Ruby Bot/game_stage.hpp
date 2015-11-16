#pragma once

#include "stage_base.hpp"

#include "maple_client.hpp"

#include "account.hpp"
#include "session.hpp"

namespace maplestory
{
	namespace stage
	{
		class game_stage : public stage_base
		{
		public:
			game_stage(std::size_t index, maple_client* client, account& account, session& session);
			~game_stage();
			
		protected:
			bool on_enter_stage();

			bool handle_packet(unsigned short header, packet_reader& packet);

		private:
			bool on_stat_changed(packet_reader& packet);
			bool on_set_field(packet_reader& packet);

		private:
			maple_client* client;

			account account;
			session session;
		};
	}
}