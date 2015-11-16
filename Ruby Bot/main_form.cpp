#include "main_form.hpp"

#include "utility.hpp"
#include "maplestory_pool.hpp"

#include <algorithm>
#include <regex>

namespace gui
{
	void main_form::update_table(std::size_t index, const std::string& key, const std::string& value)
	{
		return this->instances.at(index).second.get()->update_table(key, value);
	}
	
	void main_form::update_table(std::size_t index, const std::string& key, unsigned __int32 value)
	{
		return this->instances.at(index).second.get()->update_table(key, std::to_string(value));
	}

	void main_form::update_table(std::size_t index, const std::string& key, unsigned __int64 value)
	{
		return this->instances.at(index).second.get()->update_table(key, utility::delimited_numeric(value));
	}

	void main_form::on_recv_packet(std::size_t index, packet_reader& packet)
	{
		return this->instances.at(index).second.get()->on_recv_packet(packet);
	}

	void main_form::on_send_packet(std::size_t index, packet_writer& packet)
	{
		return this->instances.at(index).second.get()->on_send_packet(packet);
	}

	void main_form::on_clear_packets(std::size_t index)
	{
		return this->instances.at(index).second.get()->on_clear_packets();
	}

	main_form::main_form(nana::rectangle& rect) 
		: nana::form(rect, nana::appear::decorate<nana::appear::minimize>())
	{
		this->caption("Ruby Bot");
		this->icon(nana::paint::image(L"Ruby Bot.ico"));

		this->script_combox.create(*this, nana::rectangle(5, 5, 150, 20));

		WIN32_FIND_DATA find_data;
		HANDLE file = FindFirstFile(L"scripts\\*.lua", &find_data);

		if (file != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					this->script_combox.push_back(nana::string(find_data.cFileName));
				}
			} 
			while (FindNextFile(file, &find_data));
		
			FindClose(file);
		}

		if (this->script_combox.the_number_of_options() > 0)
		{
			this->script_combox.option(0);	
		}

		this->profile_listbox.create(*this, nana::rectangle(5, 30, 150, 290));
		this->profile_listbox.append_header(L"profile name", this->profile_listbox.size().width - 25);

		this->activate_profile_button.create(*this, nana::rectangle(this->profile_listbox.pos().x + this->profile_listbox.size().width + 5, 5, 20, this->size().height - 10));
		this->activate_profile_button.caption(">");
		this->activate_profile_button.events().click([this]() -> void
		{
			nana::listbox::selection selections = this->profile_listbox.selected();

			if (selections.size() >= 1)
			{
				for (nana::listbox::index_pair& i : selections)
				{
					if (ruby::maplestory_pool::get_instance().create_instance(*this->profiles.at(i.item).get(), nana::charset(this->script_combox.caption())))
					{
						static std::size_t counter = 0;
							
						this->instances.push_back(std::make_pair<std::size_t, std::unique_ptr<instance_widget>>(counter + 1, std::make_unique<instance_widget>(*this, 
							nana::rectangle(this->instance_tabbar.pos().x, this->instance_tabbar.pos().y + this->instance_tabbar.size().height, 
							this->instance_tabbar.size().width, this->size().height - (this->instance_tabbar.pos().y + this->instance_tabbar.size().height) - 5))));

						this->instance_tabbar.push_back(nana::string(nana::charset(this->profiles.at(i.item).get()->username())) + L" (" + this->script_combox.caption() + L")");
						this->instance_tabbar.relate(this->instance_tabbar.length() - 1, *this->instances.back().second.get());
						this->instance_tabbar.at(this->instance_tabbar.length() - 1) = ++counter;
					}
				}
			}
		});

		this->username_textbox.create(*this, nana::rectangle(5, profile_listbox.pos().y + profile_listbox.size().height + 5, this->profile_listbox.size().width, 20));
		this->username_textbox.tip_string(L"Enter Username...");
		this->username_textbox.multi_lines(false);
		this->username_textbox.set_accept([this](nana::char_t c) -> bool 
		{ 
			if (c == VK_TAB)
			{
				this->password_textbox.focus();
				return false;
			}

			return (this->username_textbox.caption().size() < 15); 
		});
		//this->username_textbox.caption(L"benhomes");

		this->password_textbox.create(*this, nana::rectangle(5, this->username_textbox.pos().y + this->username_textbox.size().height + 5, this->profile_listbox.size().width, 20));
		this->password_textbox.tip_string(L"Enter Password...");
		this->password_textbox.mask(L'•');
		this->password_textbox.multi_lines(false);
		this->password_textbox.set_accept([this](nana::char_t c) -> bool 
		{ 
			if (c == VK_TAB)
			{
				this->pic_textbox.focus();
				return false;
			}

			return (this->password_textbox.caption().size() < 15); 
		});
		//this->password_textbox.caption(L"asdpls");

		this->pic_textbox.create(*this, nana::rectangle(5, this->password_textbox.pos().y + this->password_textbox.size().height + 5, this->profile_listbox.size().width, 20));
		this->pic_textbox.tip_string(L"Enter PIC...");
		this->pic_textbox.mask(L'•');
		this->pic_textbox.multi_lines(false);
		this->pic_textbox.set_accept([this](nana::char_t c) -> bool	
		{ 
			if (c == VK_TAB)
			{
				this->world_combox.focus();
				return false;
			}

			return ((this->pic_textbox.caption().size() < 10 && c >= '0' && c <= '9') || c == VK_BACK);	
		});
		//this->pic_textbox.caption(L"111111");

		this->world_combox.create(*this, nana::rectangle(5, this->pic_textbox.pos().y + this->pic_textbox.size().height + 5, this->profile_listbox.size().width, 20));
		this->world_combox.push_back(L"Kradia");
		this->world_combox.push_back(L"Demethos");
		this->world_combox.option(0);
			
		this->channel_spinbox.create(*this, nana::rectangle(5, this->world_combox.pos().y + this->world_combox.size().height + 5, this->profile_listbox.size().width, 20));
		this->channel_spinbox.editable(false);
		this->channel_spinbox.range(0, 13, 1);
		this->channel_spinbox.value(L"12");

		this->character_spinbox.create(*this, nana::rectangle(5, this->channel_spinbox.pos().y + this->channel_spinbox.size().height + 5, this->profile_listbox.size().width, 20));
		this->character_spinbox.editable(false);
		this->character_spinbox.range(0, 3, 1);
			
		this->add_profile_button.create(*this, nana::rectangle(5, this->character_spinbox.pos().y + this->character_spinbox.size().height + 5, this->profile_listbox.size().width, 20));
		this->add_profile_button.caption("Add profile");
		this->add_profile_button.events().click([this]() -> void
		{
			std::string username = nana::charset(this->username_textbox.caption());
			std::string password = nana::charset(this->password_textbox.caption());
			std::string pic = nana::charset(this->pic_textbox.caption());
				
			if (!username.empty() && !password.empty() && !pic.empty())
			{
				std::size_t world = this->world_combox.option();
				std::size_t channel = this->channel_spinbox.to_int();
				std::size_t character = this->character_spinbox.to_int();

				auto doesnt_exist_already = [this](std::string username) -> bool
				{
					if (this->profiles.empty())
					{
						return true;
					}

					std::vector<std::unique_ptr<maplestory::account>>::iterator iterator = std::find_if(this->profiles.begin(), this->profiles.end(), 
						[&](std::unique_ptr<maplestory::account>& account) -> bool
					{ 
						return (utility::upper(account.get()->username()).compare(utility::upper(username)) == 0);
					});

					return (iterator == this->profiles.end());
				};

				if (doesnt_exist_already(username))
				{
					this->profiles.push_back(std::make_unique<maplestory::account>(username, password, pic, world, channel, character));
					this->profile_listbox.at(0).append(nana::charset(this->profiles.back().get()->username()));
						
					this->username_textbox.reset();
					this->password_textbox.reset();
					this->pic_textbox.reset();
				}
			}
		});

		this->instance_tabbar.create(*this, nana::rectangle(this->activate_profile_button.pos().x + this->activate_profile_button.size().width + 5, 5, 
			this->size().width - (this->activate_profile_button.pos().x + this->activate_profile_button.size().width) - 10, 20));

		this->instance_tabbar.close_fly(true);
		this->instance_tabbar.toolbox(nana::tabbar<std::size_t>::button_container<nana::tabbar<std::size_t>::button_close>(), true);
		this->instance_tabbar.events().removed([this](const nana::arg_tabbar<std::size_t>& arg)
		{
			std::size_t current_selection = this->instance_tabbar.activated();
			std::size_t selection_value = this->instance_tabbar.at(current_selection);

			for (std::size_t i = 0; i < this->instances.size(); i++)
			{
				std::size_t item_value = this->instances.at(i).first;

				if (item_value == arg.value)
				{
					auto get_username_from_tabname = [](std::string tab_string) -> std::string
					{
						if (tab_string.empty())
						{
							return std::string();
						}
							
						std::tr1::regex rgx("(.+) (.+)");
						std::tr1::smatch matches;

						return (std::tr1::regex_search(tab_string, matches, rgx) ? matches[1].str() : std::string());
					};
					
					if (ruby::maplestory_pool::get_instance().delete_instance(get_username_from_tabname(nana::charset(this->instance_tabbar.text(i)))))
					{
						if (item_value == selection_value && this->instances.size() > 1)
						{
							if (this->instances.size() - 1 > current_selection)
							{
								current_selection++;
							}
							else
							{
								current_selection--;
							}

							this->instance_tabbar.activate(current_selection);
						}

						this->instances.erase(this->instances.begin() + i);
					}
				}
			}
		});
	}
}