#pragma once

#include <Windows.h>

#define WM_SOCKET	WM_USER + 1
#define WM_NMCO		WM_USER + 2

namespace maplestory
{
	namespace network
	{
		class tcp_handler
		{
		public:
			static tcp_handler& get_instance()
			{
				static tcp_handler handler;
				return handler;
			}

			bool initialize(HINSTANCE instance);
			bool initialize_nmco();

			HWND get_handle();

		private:
			static LRESULT CALLBACK window_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

			bool register_class(HINSTANCE instance);
			bool create_handler(HINSTANCE instance);

			HWND hwnd;
		};
	}
}