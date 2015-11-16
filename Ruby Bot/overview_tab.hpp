#pragma once

#include <nana\gui.hpp>
#include <nana\gui\widgets\listbox.hpp>
#include <nana\gui\widgets\panel.hpp>

#include <string>

namespace gui
{
	class overview_tab : public nana::panel<false>
	{
	public:
		overview_tab(nana::window wd, nana::rectangle& rect);

		void update_table(const std::string& key, const std::string& value);

	private:
		std::size_t get_index_by_key(std::string const& key);

		nana::listbox overview_listbox;
	};
}