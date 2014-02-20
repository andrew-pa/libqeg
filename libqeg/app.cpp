#include "app.h"
#include "timer.h"
#include "basic_input.h"
#include <windowsx.h>

namespace qeg
{
#ifdef WIN32
	static app* this_app;
	static bool size_changed;

	app::app(const wstring& title, vec2 winsize, bool vfps, float tmpf)
		: fps(0), mpf(0), var_fps(vfps), targ_mpf(tmpf)
	{
		this_app = this;
		size_changed = false;
		//Create Window
		WNDCLASS windowClass;
		DWORD exsty = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		auto inst = GetModuleHandle(nullptr);

		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
#pragma region WNDPROC
		windowClass.lpfnWndProc = [](HWND _wnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT
		{
			if (msg == WM_SIZE)
			{
				this_app->_dev->resize(vec2(LOWORD(lp), HIWORD(lp)));
				size_changed = true;
				return 0;
			}
			else if(msg == WM_MOUSEMOVE || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP 
				|| msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP || msg == WM_RBUTTONDOWN 
				|| msg == WM_RBUTTONUP)
			{
				input::mouse::__update(vec2(GET_X_LPARAM(lp), GET_Y_LPARAM(lp)), 
					check_flag(wp, MK_LBUTTON), check_flag(wp, MK_MBUTTON), check_flag(wp, MK_RBUTTON));
			}
			else if(msg == WM_KEYDOWN)
			{
				input::keyboard::__update((input::key)wp, true);
			}
			else if (msg == WM_KEYUP)
			{
				input::keyboard::__update((input::key)wp, false);
			}
			else if (msg == WM_DESTROY)
			{
				PostQuitMessage(0);
				return 0;
			}
			return DefWindowProc(_wnd, msg, wp, lp);
		};
#pragma endregion
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = inst;
		windowClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = nullptr;
		windowClass.lpszMenuName = nullptr;
		windowClass.lpszClassName = title.c_str();

		RegisterClass(&windowClass);

		wnd = CreateWindowEx(exsty, title.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, (int)winsize.x, (int)winsize.y, nullptr, nullptr, inst, nullptr);

		_dev = new device(winsize, wnd
#ifdef DIRECTX
			, 4
#endif
			);
		ShowWindow(wnd, SW_SHOW);
		UpdateWindow(wnd);
	}

	void app::run()
	{
		MSG msg{ 0 };
		timer tm;
		uint fc = 0;
		float ft = 0.f;
		while (true)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) return;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tm.update();
				if (size_changed)
				{
					resized();
					size_changed = false;
				}
				update(tm.total_time(), tm.delta_time());
				_dev->update_render_target();
				render(tm.total_time(), tm.delta_time());
				_dev->present();
				fc++;
				ft += tm.delta_time();
				if (ft > 1.0f)
				{
					fps = (float)fc;
					mpf = 1000.f / fps;
					cdlog << ":FPS: FPS = " << fps << "\tMPF = " << mpf << endl;
				    //OutputDebugString(wos.str().c_str());
					fc = 0;
					ft = 0.0f;
				}
				if (!var_fps)
				{
					if (tm.delta_time() < targ_mpf)
					{
						float missing = targ_mpf - tm.delta_time();
						Sleep((DWORD)ceil(missing * 1000));
					}
				}
				//Sleep(10);
			}
		}
	}
#endif
}