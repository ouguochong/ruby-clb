#include "packet_flow_tab.hpp"

#include <iomanip>

namespace gui
{
	packet_flow_tab::packet_flow_tab(nana::window wd, nana::rectangle& rect)
		: nana::panel<false>(wd, rect, true)
	{
		/* packet listbox */
		this->packet_listbox.create(*this, nana::rectangle(0, 5, this->size().width, this->size().height - 35));
		this->packet_listbox.typeface(nana::paint::font(L"Consolas", 10));
		this->packet_listbox.enable_single(true, false);

		this->packet_listbox.append_header(L"Type", 45);
		this->packet_listbox.append_header(L"Size", 50);
		this->packet_listbox.append_header(L"Data", this->size().width - 120);

		/* packet clear button */
		this->packet_reset_button.create(*this, nana::rectangle(0, this->size().height - 25, this->size().width, 25));
		this->packet_reset_button.caption(L"Clear");
		this->packet_reset_button.events().click([this]() -> void
		{
			this->clear_list();
		});
	}
		
	void packet_flow_tab::on_recv_packet(packet_reader& packet)
	{
		this->packet_listbox.at(0).append({ L"Recv", std::to_wstring(packet.get_size()), this->parse_packet(packet.get_vector()) });
		this->packet_listbox.at(0).back().fgcolor(nana::colors::cornflower_blue);
	}
		
	void packet_flow_tab::on_send_packet(packet_writer& packet)
	{
		this->packet_listbox.at(0).append({ L"Send", std::to_wstring(packet.get_size()), this->parse_packet(packet.get_vector()) });
		this->packet_listbox.at(0).back().fgcolor(nana::colors::dark_sea_green);
	}
	
	void packet_flow_tab::clear_list()
	{
		this->packet_listbox.clear();
	}
	
	nana::string packet_flow_tab::parse_packet(std::vector<unsigned char>& packet_data)
	{
		std::wstringstream ss;
		
		for (std::size_t i = 0; i < packet_data.size(); i++)
		{
			if (i != 0)
			{
				ss << " ";
			}
			
			ss << std::uppercase << std::hex << std::setfill(L'0') << std::setw(2) << static_cast<unsigned int>(packet_data.at(i));
		}

		return ss.str();
	}
}