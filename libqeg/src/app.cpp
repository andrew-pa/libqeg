#include "app.h"
#include "timer.h"
#include "basic_input.h"
#if defined(WIN32) && defined(DIRECTX)
#include <windowsx.h>
#include <codecvt>
#endif 
namespace qeg
{
#if defined(OPENGL)
#define null_key input::key(-1)
#define k(x) input::key::##x
	input::key translate_key(int gk)
	{
		if (gk == GLFW_KEY_UNKNOWN) return null_key;
		else if (gk == GLFW_KEY_SPACE) return input::key::space;
		else if (gk == GLFW_KEY_APOSTROPHE) return input::key::quote;
		else if(gk >= GLFW_KEY_COMMA && gk <= GLFW_KEY_9)
		{
			static const input::key table_44_to_57[] =
			{
				k(comma),
				k(minus),
				k(period),
				k(fwdslash),
				k(key_0),
				k(key_1),
				k(key_2),
				k(key_3),
				k(key_4),
				k(key_5),
				k(key_6),
				k(key_7),
				k(key_8),
				k(key_9),
			};
			return table_44_to_57[gk - GLFW_KEY_COMMA];
		}
		else if (gk == GLFW_KEY_SEMICOLON) return k(colon);
		else if (gk == GLFW_KEY_EQUAL) return k(plus);
		else if (gk >= GLFW_KEY_A && gk <= GLFW_KEY_Z) return input::key(gk);
		else if (gk == GLFW_KEY_LEFT_BRACKET) return k(lbracket);
		else if (gk == GLFW_KEY_BACKSLASH) return k(bckslash);
		else if (gk == GLFW_KEY_GRAVE_ACCENT) return k(backtick);
		else if(gk >= GLFW_KEY_ESCAPE && gk <= GLFW_KEY_PAUSE)
		{
			static const input::key table_256_to_284[] =
			{
				k(escape),
				k(enter),
				k(tab),
				k(back),
				k(insert),
				k(deletek),
				k(right),
				k(left),
				k(down),
				k(up),
				k(page_up),
				k(page_down),
				k(home),
				k(end),
				k(capslock),
				k(scrllock),
				k(numlock),
				k(print),
				k(pause),
			};
			return table_256_to_284[gk - GLFW_KEY_ESCAPE];
		}
		else if(gk >= GLFW_KEY_F1 && gk <= GLFW_KEY_F24)
		{
			return input::key((short)input::key::f1 + (short)(gk - GLFW_KEY_F1));
		}
		else if(gk >= GLFW_KEY_KP_0 && gk <= GLFW_KEY_LAST)
		{
			static const input::key table_last[] =
			{
				k(numpad0),
				k(numpad1),
				k(numpad2),
				k(numpad3),
				k(numpad4),
				k(numpad5),
				k(numpad6),
				k(numpad7),
				k(numpad8),
				k(numpad9),
				k(period),
				k(divide),
				k(multiply),
				k(subtract),
				k(enter),
				k(plus),
				k(lshift),
				k(lctrl),
				k(lmenu),
				null_key,
				k(rshift),
				k(rctrl),
				k(rmenu),
				null_key,
				k(menu)
			};
			return table_last[gk - GLFW_KEY_KP_0];
		}
		return input::key(-1);
	}
#undef k
#endif
#ifdef WIN32
	static app* this_app;
	static bool size_changed;
#ifdef DIRECTX
	wstring s2ws(const string& str)
	{
		typedef codecvt_utf8<wchar_t> convert_typeX;
		wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
	}

	string ws2s(const wstring& wstr)
	{
		typedef codecvt_utf8<wchar_t> convert_typeX;
		wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}
#endif
#endif
	app::app(const string& title, vec2 winsize, uint aa_samples, bool vfps, float tmpf)
		: fps(0), mpf(0), var_fps(vfps), targ_mpf(tmpf), _title(title)
	{
#ifdef OPENGL
		targ_mpf *= .5f;
#elif DIRECTX
		var_fps = true;
#endif
#if defined(WIN32) && defined(DIRECTX)
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
				input::mouse::__update(
					(vec2(GET_X_LPARAM(lp), GET_Y_LPARAM(lp)) / this_app->_dev->size()) - vec2(.5f), 
					check_flag(MK_LBUTTON, (int)wp), check_flag(MK_MBUTTON, (int)wp), check_flag(MK_RBUTTON, (int)wp));
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
		auto t = s2ws(title);
		windowClass.lpszClassName = t.c_str();

		RegisterClass(&windowClass);

		wnd = CreateWindowEx(exsty, t.c_str(), t.c_str(), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, (int)winsize.x, (int)winsize.y, nullptr, nullptr, inst, nullptr);

		_dev = new device(winsize, wnd, aa_samples);
		ShowWindow(wnd, SW_SHOW);
		UpdateWindow(wnd);
#endif
#ifdef OPENGL
		if (!glfwInit())
			throw exception("GLFW init failed");
		
		glfwWindowHint(GLFW_SAMPLES, aa_samples);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

		wnd = glfwCreateWindow(winsize.x, winsize.y, title.c_str(), nullptr, nullptr);
		if(!wnd)
		{
			glfwTerminate();
			throw exception("GLFW window creation failed");
		}

		glfwSetWindowUserPointer(wnd, this);

		glfwSetKeyCallback(wnd, [](GLFWwindow* w, int key, int scancode, int action, int mods) 
		{
			auto p = (app*)glfwGetWindowUserPointer(w);
			input::keyboard::__update(translate_key(key), action == GLFW_PRESS || action == GLFW_REPEAT);
		});

		_dev = new device(winsize, wnd, aa_samples);
#endif
	}

	app::~app()
	{
#ifdef OPENGL
		glfwTerminate();
#endif
		delete _dev;
	}

	void app::run()
	{
#if defined(WIN32) && defined(DIRECTX)
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
					wostringstream wos;
					wos << s2ws(_title) << " | fps: " << fps << "  mspf: " << mpf;
					wos << " | dt: " << tm.delta_time();
					SetWindowText(wnd, wos.str().c_str());

					//OutputDebugString(wos.str().c_str());
					fc = 0;
					ft = 0.0f;
				}
				if (!var_fps)
				{
					if (tm.delta_time() < targ_mpf)
					{
						float missing = targ_mpf - tm.delta_time();
						::this_thread::sleep_for(chrono::milliseconds((long)ceil(missing)));
					}
				}	
				//Sleep(10);
			}
		}
#elif OPENGL
		timer tm;
		uint fc = 0;
		float ft = 0.f;
		while(!glfwWindowShouldClose(wnd))
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
				ostringstream wos;
				wos << _title << " | fps: " << fps << "  mspf: " << mpf;
				wos << " | dt: " << tm.delta_time();
				glfwSetWindowTitle(wnd, wos.str().c_str());

				//OutputDebugString(wos.str().c_str());
				fc = 0;
				ft = 0.0f;
			}
			glfwPollEvents();
			if (!var_fps)
			{
				if (tm.delta_time() < targ_mpf)
				{
					float missing = targ_mpf - tm.delta_time();
					this_thread::sleep_for(std::chrono::milliseconds((long)ceil(missing)));
				}
			}
		}
#endif
	}
}