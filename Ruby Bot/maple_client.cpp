#include "maple_client.hpp"

#include "config.hpp"

#include "game_stage.hpp"
#include "login_stage.hpp"

#include "main_form.hpp"

namespace maplestory
{
	maple_client::maple_client(maplestory::account& account, std::size_t index, const std::string& script_file)
		: account_(account), index_(index)
	{
		this->current_stage.reset();
		this->script_manager.load_script(script_file);
	}

	maple_client::~maple_client()
	{
		this->current_stage.reset();
	}
	
	bool maple_client::reset()
	{
		return this->migrate(server_type::login, config::server::ip, config::server::port);
	}

	bool maple_client::migrate(server_type type, std::string const& ip, unsigned short port)
	{
		this->current_stage.reset();
			
		switch (type)
		{
		case server_type::game:
			this->current_stage = std::make_unique<stage::game_stage>(this->get_index(), this, this->account(), this->session());
			this->script_manager.update_stage(this->current_stage.get());
			break;

		case server_type::login:
			this->current_stage = std::make_unique<stage::login_stage>(this->get_index(), this, this->account());
			break;

		default:
			printf("Unknown stage requested!\n");
			break;
		}
		
		return (this->current_stage.get() && this->current_stage.get()->connect(ip, port));
	}
	
	bool maple_client::notify_close(SOCKET desc)
	{
		//gui::main_form::get_instance().on_clear_packets(this->get_index());
		return this->reset();
	}

	bool maple_client::notify_connect(SOCKET desc)
	{
		return true;
	}
	
	bool maple_client::notify_read(SOCKET desc)
	{
		if (!this->current_stage.get()->try_read())
		{
			return this->reset();
		}

		return true;
	}

	bool maple_client::notify_write(SOCKET desc)
	{
		return true;
	}

	SOCKET maple_client::get_desc() const
	{
		return (this->current_stage.get() ? this->current_stage.get()->get_desc() : INVALID_SOCKET);
	}

	lua_script& maple_client::get_script_manager()
	{
		return this->script_manager;
	}

	std::size_t maple_client::get_index() const
	{
		return this->index_;
	}
	
	void maple_client::set_index(std::size_t index)
	{
		this->current_stage.get()->reset_index(this->index_ = index);
	}
	
	account& maple_client::account()
	{
		return this->account_;
	}

	session& maple_client::session()
	{
		return this->session_;
	}
}