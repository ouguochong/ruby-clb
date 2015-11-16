#pragma once

#include <nana\gui.hpp>
#include <nana\gui\widgets\button.hpp>
#include <nana\gui\widgets\listbox.hpp>
#include <nana\gui\widgets\panel.hpp>

#include "packet_reader.hpp"
#include "packet_writer.hpp"

#include <string>

namespace gui
{
	class packet_flow_tab : public nana::panel<false>
	{
	public:
		packet_flow_tab(nana::window wd, nana::rectangle& rect);
		
		void on_recv_packet(packet_reader& packet);
		void on_send_packet(packet_writer& packet);

		void clear_list();

	private:
		nana::string parse_packet(std::vector<unsigned char>& packet_data);

		nana::listbox packet_listbox;
		nana::button packet_reset_button;
	};
}