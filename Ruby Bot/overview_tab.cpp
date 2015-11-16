#include "overview_tab.hpp"
#include "utility.hpp"

namespace gui
{
	overview_tab::overview_tab(nana::window wd, nana::rectangle& rect)
		: nana::panel<false>(wd, rect, true)
	{
		this->overview_listbox.create(*this, nana::rectangle(0, 5, this->size().width, this->size().height - 5));

		this->overview_listbox.append_header(L"key", static_cast<unsigned int>((this->size().width - 25) / 2));
		this->overview_listbox.append_header(L"value", static_cast<unsigned int>((this->size().width - 25) / 2));
	}

	void overview_tab::update_table(const std::string& key, const std::string& value)
	{
		nana::listbox::cat_proxy cat = this->overview_listbox.at(0);
		std::size_t index_by_key = this->get_index_by_key(key);

		if (index_by_key != static_cast<std::size_t>(-1))
		{
			cat.at(index_by_key).text(1, nana::string(nana::charset(value)));
		}
		else
		{
			cat.append({ nana::string(nana::charset(key)), nana::string(nana::charset(value)) });
		}
	}

	std::size_t overview_tab::get_index_by_key(std::string const& key)
	{
		nana::listbox::cat_proxy cat = this->overview_listbox.at(0);

		for (std::size_t i = 0; i < cat.size(); i++)
		{
			std::string item_key = nana::charset(cat.at(i).text(0));

			if (utility::lower(item_key).compare(utility::lower(key)) == 0)
			{
				return i;
			}
		}

		return static_cast<std::size_t>(-1);
	}
}