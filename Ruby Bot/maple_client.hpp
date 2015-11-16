#pragma once

#include "account.hpp"
#include "session.hpp"

#include "lua_script.hpp"
#include "stage_base.hpp"

namespace maplestory
{
	enum class server_type
	{
        login,
		game,
	};

	class maple_client
	{
	public:
		maple_client(maplestory::account& account, std::size_t index, const std::string& script_file);
		~maple_client();

		bool reset();

		bool migrate(server_type type, std::string const& ip, unsigned short port);
	
		bool notify_close(SOCKET desc);
		bool notify_connect(SOCKET desc);
		bool notify_read(SOCKET desc);
		bool notify_write(SOCKET desc);

		SOCKET get_desc() const;

		lua_script& get_script_manager();

		std::size_t get_index() const;
		void set_index(std::size_t index);

		maplestory::account& account();
		maplestory::session& session();

	private:
		std::size_t index_;
		lua_script script_manager;

		maplestory::account account_;
		maplestory::session session_;
		
		std::unique_ptr<stage::stage_base> current_stage;
	};
}