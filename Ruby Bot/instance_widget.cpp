#include "instance_widget.hpp"

#include "overview_tab.hpp"
#include "packet_flow_tab.hpp"

namespace gui
{
	instance_widget::instance_widget(nana::window wd, nana::rectangle& rect) 
		: nana::panel<true>(wd, rect, false)
	{
		this->tabbar.create(*this, nana::rectangle(0, 5, this->size().width, 20));
			
		const nana::string tabnames[] = 
		{
			L"Overview",
			L"Packet Flow",
		};

		std::size_t start_position_y = this->tabbar.pos().y + this->tabbar.size().height;

		for (std::size_t i = 0; i < _countof(tabnames); i++)
		{
			this->tabbar.push_back(tabnames[i]);

			switch (i)
			{
			case 0:
				this->tabpages.push_back(std::make_unique<overview_tab>(*this, nana::rectangle(0, start_position_y, this->tabbar.size().width, this->size().height - start_position_y)));
				break;
					
			case 1:
				this->tabpages.push_back(std::make_unique<packet_flow_tab>(*this, nana::rectangle(0, start_position_y, this->tabbar.size().width, this->size().height - start_position_y)));
				break;
					
			default:
				this->tabpages.push_back(std::make_unique<nana::panel<false>>(*this, nana::rectangle(0, start_position_y, this->tabbar.size().width, this->size().height - start_position_y)));
				break;
			}

			this->tabbar.relate(i, *this->tabpages[this->tabpages.size() - 1].get());
		}
		
		this->tabbar.activate(0);
	}
	
	void instance_widget::update_table(const std::string& key, const std::string& value)
	{
		return reinterpret_cast<overview_tab*>(this->tabpages[0].get())->update_table(key, value);
	}

	void instance_widget::on_recv_packet(packet_reader& packet)
	{
		return reinterpret_cast<packet_flow_tab*>(this->tabpages[1].get())->on_recv_packet(packet);
	}

	void instance_widget::on_send_packet(packet_writer& packet)
	{
		return reinterpret_cast<packet_flow_tab*>(this->tabpages[1].get())->on_send_packet(packet);
	}

	void instance_widget::on_clear_packets()
	{
		return reinterpret_cast<packet_flow_tab*>(this->tabpages[1].get())->clear_list();
	}
}