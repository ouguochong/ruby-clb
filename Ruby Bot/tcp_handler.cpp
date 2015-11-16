#include "tcp_handler.hpp"

#include "maplestory_pool.hpp"

#include "nexon_api\NMCOClientObject.h"

namespace maplestory
{
	namespace network
	{
		bool tcp_handler::initialize(HINSTANCE instance)
		{
			return (this->register_class(instance) && this->create_handler(instance));
		}
		
		bool tcp_handler::initialize_nmco()
		{
			return (CNMCOClientObject::GetInstance().RegisterCallbackMessage(this->hwnd, WM_NMCO) != FALSE);
		}

		HWND tcp_handler::get_handle()
		{
			return this->hwnd;
		}

		LRESULT CALLBACK tcp_handler::window_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			if (message == WM_SOCKET)
			{
				if (WSAGETSELECTERROR(lParam))
				{
					printf("Disconnected from server (WSAGETSELECTERROR - %d)\n", WSAGETSELECTERROR(lParam));
					return static_cast<LRESULT>(ruby::maplestory_pool::get_instance().on_socket(static_cast<SOCKET>(wParam), FD_CLOSE));
				}

				return static_cast<LRESULT>(ruby::maplestory_pool::get_instance().on_socket(static_cast<SOCKET>(wParam), WSAGETSELECTEVENT(lParam)));
			}
			else if (message == WM_NMCO)
			{
				printf("NMCO_MSG: %08X, %08X\n", wParam, lParam);
				return static_cast<LRESULT>(0);
			}

			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		bool tcp_handler::register_class(HINSTANCE instance)
		{
			WNDCLASSEXW wcex;
			memset(&wcex, 0, sizeof(WNDCLASSEX));

			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.lpfnWndProc = tcp_handler::window_proc;
			wcex.hInstance = instance;
			wcex.lpszClassName = L"tcp_handler_class";

			return (RegisterClassExW(&wcex) != NULL);
		}

		bool tcp_handler::create_handler(HINSTANCE instance)
		{
			return ((this->hwnd = CreateWindowExW(NULL, L"tcp_handler_class", NULL, NULL, CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT, HWND_MESSAGE, NULL, instance, NULL)) != NULL);
		}
	}
}