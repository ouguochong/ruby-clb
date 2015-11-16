#pragma once

#include <nana\gui.hpp>
#include <nana\gui\widgets\listbox.hpp>
#include <nana\gui\widgets\panel.hpp>
#include <nana\gui\widgets\tabbar.hpp>

#include "packet_reader.hpp"
#include "packet_writer.hpp"

#include <string>

namespace gui
{
	class instance_widget : public nana::panel<true>
	{
	public:
		instance_widget(nana::window wd, nana::rectangle& rect);
		
		void update_table(const std::string& key, const std::string& value);

		void on_recv_packet(packet_reader& packet);
		void on_send_packet(packet_writer& packet);
		void on_clear_packets();

	private:
		nana::tabbar<std::size_t> tabbar;
		std::vector<std::unique_ptr<nana::panel<false>>> tabpages;
	};
}