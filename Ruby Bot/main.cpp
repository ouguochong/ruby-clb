#include "main_form.hpp"

#include "nexon_api\NMCOClientObject.h"
#include "nexon_api\NMManager.h"

#include "tcp_handler.hpp"

namespace nm
{
	bool initialize_authentication()
	{
		CNMManager::GetInstance().Init();
	
		if (!CNMCOClientObject::GetInstance().SetLocale(kLocaleID_EU, kRegionCode_EU3))
		{
			return false;
		}
	
		if (!CNMCOClientObject::GetInstance().Initialize(kGameCode_maplestory_eu))
		{
			return false;
		}

		return true;
	}
}

namespace wsa
{
	bool initialize()
	{
		WSADATA wsa_data;
		memset(&wsa_data, 0, sizeof(WSADATA));

		return (WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0 && wsa_data.wVersion == MAKEWORD(2, 2));
	}

	bool uninitialize()
	{
		return (WSACleanup() == 0);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* szCommandLine, int nCommandShow)
{
	HANDLE host_mutex = CreateMutex(NULL, FALSE, "ruby_bot_mtx");

	if (!host_mutex || host_mutex == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return 0;
	}
	
	if (!maplestory::network::tcp_handler::get_instance().initialize(hInstance))
	{
		ReleaseMutex(host_mutex);
		return 0;
	}

	if (!nm::initialize_authentication())
	{
		ReleaseMutex(host_mutex);
		return 0;
	}
	
	if (!maplestory::network::tcp_handler::get_instance().initialize_nmco())
	{
		ReleaseMutex(host_mutex);
		return 0;
	}
	
	if (!wsa::initialize())	
	{
		ReleaseMutex(host_mutex);
		return 0;
	}
	
	AllocConsole();
	SetConsoleTitle("NIGGAPLEREASE");
	AttachConsole(GetCurrentProcessId());
	
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CON", "r", stdin);
	freopen_s(&pFile, "CON", "w", stdout);
	freopen_s(&pFile, "CON", "w", stderr);
	
	srand(static_cast<int>(time(NULL)));

	gui::main_form::get_instance().show();
	nana::exec();
	
	wsa::uninitialize();
	ReleaseMutex(host_mutex);
	FreeConsole();
	return 0;
}