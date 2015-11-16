#pragma once

#include "maple_client.hpp"

#include <functional>

namespace ruby
{
	class maplestory_pool
	{
	public:
		static maplestory_pool& get_instance()
		{
			static maplestory_pool manager_pool;
			return manager_pool;
		}

		bool create_instance(maplestory::account& account, const std::string& script_file);
		bool delete_instance(std::string const& username);
		
		bool on_socket(SOCKET desc, unsigned short type);

	private:
		maplestory_pool();
		virtual ~maplestory_pool();
		
		bool find(std::string const& username, std::function<bool(std::vector<std::shared_ptr<maplestory::maple_client>>::iterator& iterator)> fn = 
			[](std::vector<std::shared_ptr<maplestory::maple_client>>::iterator& iterator) -> bool { return true; });

		bool find(SOCKET desc, std::function<bool(std::vector<std::shared_ptr<maplestory::maple_client>>::iterator& iterator)> fn = 
			[](std::vector<std::shared_ptr<maplestory::maple_client>>::iterator& iterator) -> bool { return true; });

	private:
		std::vector<std::shared_ptr<maplestory::maple_client>> pool;
	};
}