#include "maplestory_pool.hpp"

#include "utility.hpp"

#include <algorithm>

namespace ruby
{
	bool maplestory_pool::create_instance(maplestory::account& account, const std::string& script_file)
	{
		if (this->find(account.username()))
		{
			return false;
		}

		std::shared_ptr<maplestory::maple_client> maplestory(std::make_shared<maplestory::maple_client>(account, this->pool.size(), script_file));

		if (!maplestory.get()->reset())
		{
			return false;
		}

		this->pool.push_back(maplestory);
		return true;
	}
	
	bool maplestory_pool::delete_instance(std::string const& username)
	{
		return this->find(username, [this](std::vector<std::shared_ptr<maplestory::maple_client>>::iterator& iterator) -> bool
		{
			this->pool.erase(iterator);

			for (std::size_t i = 0; i < this->pool.size(); i++)
			{
				this->pool.at(i).get()->set_index(i);
			}

			return true;
		});
	}
	
	bool maplestory_pool::on_socket(SOCKET desc, unsigned short type)
	{
		return this->find(desc, [&](std::vector<std::shared_ptr<maplestory::maple_client>>::iterator& iterator) -> bool
		{
			switch (type)
			{
			case FD_CLOSE:
				return (*iterator).get()->notify_close(desc);

			case FD_CONNECT:
				return (*iterator).get()->notify_connect(desc);

			case FD_READ:
				return (*iterator).get()->notify_read(desc);

			case FD_WRITE:
				return (*iterator).get()->notify_write(desc);

			default:
				break;
			}

			return false;
		});
	}

	maplestory_pool::maplestory_pool()
	{
		this->pool.clear();
	}

	maplestory_pool::~maplestory_pool()
	{
		this->pool.clear();
	}
	
	bool maplestory_pool::find(std::string const& username, std::function<bool(std::vector<std::shared_ptr<maplestory::maple_client>>::iterator& iterator)> fn)
	{
		if (this->pool.empty())
		{
			return false;
		}

		std::vector<std::shared_ptr<maplestory::maple_client>>::iterator iterator = std::find_if(this->pool.begin(), this->pool.end(), 
			[&](std::shared_ptr<maplestory::maple_client> const& maplestory) -> bool 
		{ 
			return (utility::upper(username).compare(utility::upper(maplestory.get()->account().username())) == 0);
		});

		if (iterator == this->pool.end())
		{
			return false;	
		}

		return fn(iterator);
	}
	
	bool maplestory_pool::find(SOCKET desc, std::function<bool(std::vector<std::shared_ptr<maplestory::maple_client>>::iterator& iterator)> fn)
	{
		if (this->pool.empty())
		{
			return false;
		}

		std::vector<std::shared_ptr<maplestory::maple_client>>::iterator iterator = std::find_if(this->pool.begin(), this->pool.end(), 
			[&](std::shared_ptr<maplestory::maple_client> const& maplestory) -> bool 
		{ 
			return (desc == maplestory.get()->get_desc());
		});

		if (iterator == this->pool.end())
		{
			return false;	
		}

		return fn(iterator);
	}
}