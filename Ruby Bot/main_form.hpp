#pragma once

#include <nana\gui.hpp>

#include <nana\gui\widgets\button.hpp>
#include <nana\gui\widgets\combox.hpp>
#include <nana\gui\widgets\listbox.hpp>
#include <nana\gui\widgets\spinbox.hpp>
#include <nana\gui\widgets\tabbar.hpp>
#include <nana\gui\widgets\textbox.hpp>

#include "account.hpp"
#include "instance_widget.hpp"

#include <memory>

namespace gui
{
	class main_form : public nana::form
	{
	public:
		static main_form& get_instance()
		{
			static main_form main_window(nana::API::make_center(800, 500));
			return main_window;
		}
		
		void update_table(std::size_t index, const std::string& key, const std::string& value);
		void update_table(std::size_t index, const std::string& key, unsigned __int32 value);
		void update_table(std::size_t index, const std::string& key, unsigned __int64 value);

		void on_recv_packet(std::size_t index, packet_reader& packet);
		void on_send_packet(std::size_t index, packet_writer& packet);
		void on_clear_packets(std::size_t index);

	private:
		main_form(nana::rectangle& rect);
		
	private:
		nana::combox script_combox;

		nana::listbox profile_listbox;
		std::vector<std::unique_ptr<maplestory::account>> profiles;

		nana::button activate_profile_button;

		nana::textbox username_textbox;
		nana::textbox password_textbox;
		nana::textbox pic_textbox;
		nana::combox world_combox;
		nana::spinbox channel_spinbox;
		nana::spinbox character_spinbox;
	
		nana::button add_profile_button;

		nana::tabbar<std::size_t> instance_tabbar;
		std::vector<std::pair<std::size_t, std::unique_ptr<instance_widget>>> instances;
	};
}