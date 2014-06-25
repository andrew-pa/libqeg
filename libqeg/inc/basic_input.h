#pragma once
#include "cmmn.h"

#ifdef WIN32
#include <Xinput.h>
#endif

namespace qeg
{
	namespace input
	{
		class mouse
		{
		public:
			struct state
			{
				vec2 pos;
				bool left, middle, right;
				state() {}
				state(vec2 p, bool l, bool m, bool r)
					: pos(p), left(l), middle(m), right(r)
				{}
			};
			static const state get_state();

			static const state get_delta_state();

			static void __update(vec2 p, bool left, bool middle, bool right);
		};

#ifdef WIN32
		enum class key : short
		{
			//copied from WinUser.h's VK_* macros
              back     =      0x08,
              tab      =      0x09,

              clear     =     0x0c,
              enter     =	  0x0d,

              shift     =     0x10,
              control   =     0x11,
              menu      =     0x12,
              pause     =     0x13,
              capslock   =     0x14,

              escape      =   0x1b,

              space       =   0x20,
              page_up     =   0x21,
              page_down   =   0x22,
              end         =   0x23,
              home        =   0x24,
              left        =   0x25,
              up          =  0x26,
              right       =   0x27,
              down        =   0x28,
              selectk      =   0x29,
              print       =   0x2a,
              execute     =   0x2b,
              snapshot    =   0x2c,
              insert      =   0x2d,
              deletek      =   0x2e,
              help        =   0x2f,
			  
			  key_0 = 0x30,
			  key_1 = 0x31,
			  key_2 = 0x32,
			  key_3 = 0x33,
			  key_4 = 0x34,
			  key_5 = 0x35,
			  key_6 = 0x36,
			  key_7 = 0x37,
			  key_8 = 0x38,
			  key_9 = 0x39,

			  key_a = 0x41,
			  key_b = 0x42,
			  key_c = 0x43,
			  key_d = 0x44,
			  key_e = 0x45,
			  key_f = 0x46,
			  key_g = 0x47,
			  key_h = 0x48,
			  key_i = 0x49,
			  key_j = 0x4a,
			  key_k = 0x4b,
			  key_l = 0x4c,
			  key_m = 0x4d,
			  key_n = 0x4e,
			  key_o = 0x4f,
			  key_p = 0x50,
			  key_q = 0x51,
			  key_r = 0x52,
			  key_s = 0x53,
			  key_t = 0x54,
			  key_u = 0x55,
			  key_v = 0x56,
			  key_w = 0x57,
			  key_x = 0x58,
			  key_y = 0x59,
			  key_z = 0x5a,

              lwin       =  0x5b,
              rwin       =  0x5c,
              apps       =  0x5d,

              sleep      =  0x5f,
			  
              numpad0    =  0x60,
              numpad1    =  0x61,
              numpad2    =  0x62,
              numpad3    =  0x63,
              numpad4    =  0x64,
              numpad5    =  0x65,
              numpad6    =  0x66,
              numpad7    =  0x67,
              numpad8    =  0x68,
              numpad9    =  0x69,
              multiply   =  0x6a,
              add        =  0x6b,
              separator  =  0x6c,
              subtract   =  0x6d,
              decimal    =  0x6e,
              divide     =  0x6f,
              f1         =  0x70,
              f2         =  0x71,
              f3         =  0x72,
              f4         =  0x73,
              f5         =  0x74,
              f6         =  0x75,
              f7         =  0x76,
              f8         =  0x77,
              f9         =  0x78,
              f10        =  0x79,
              f11        =  0x7a,
              f12        =  0x7b,
              f13        =  0x7c,
              f14        =  0x7d,
              f15        =  0x7e,
              f16        =	0x7f,
              f17        =	0x80,
              f18        =	0x81,
              f19        =	0x82,
              f20        =	0x83,
              f21        =	0x84,
              f22        =	0x85,
              f23        =	0x86,
              f24		 =	0x87,

              numlock    =	0x90,
              scrllock   =	0x91,

			  lshift	 =	0xA0,
			  rshift,
			  lctrl,
			  rctrl,
			  lmenu,
			  rmenu,

			  colon		 =	0xBA, //this is VK_OEM_1, it could be anything, but it's supposed to be the ';:' key
			  plus		 =	0xBB, 
			  comma,
			  minus,
			  period,
			  fwdslash,	//this is VK_OEM_2, like VK_OEM_1. Std says it's '/?' key
			  backtick, //this is like VK_OEM_1. Is '`~' key (VK_OEM_3)
			  lbracket, // Std = '[{' key 
			  bckslash, // Std = '\|' key
			  rbracket, // Std = ']}' key
			  quote,	// Std = ' '" ' key (single/double quote)

		};

#endif
		
		class keyboard
		{
		public:
			struct state
			{
			public:
				bool key_down(key k) const;
				bool key_pressed(key k) const;
			};
			inline static const state get_state() { return state();  }

			static void __update(key kc, bool s);
		};

		struct gamepad_not_connected_exception : public exception
		{
			inline const char* what() const override
			{
				return "Gamepad not connected";
			}
		};

		class gamepad
		{
#ifdef WIN32
			DWORD _usr_idx;
			XINPUT_STATE _cxis;
			XINPUT_STATE _lxis;
			bool _connected;
#endif
			inline static float dead_zone(short val, short dead_z, short mx = numeric_limits<short>::max())
			{
				float vn = clamp((float)val / (float)mx, -1.f, 1.f);
				if (abs(vn) < ((float)dead_z / (float)mx))
					vn = 0.f;
				return vn;
			}
		public:
			enum class button
			{
				A, B, X, Y,
				start, back, home,
				left_bumper, right_bumper,
				left_stick, right_stick,
				dpad_left, dpad_right, 
				dpad_up, dpad_down,
			};

			struct state
			{
#ifdef WIN32
				XINPUT_STATE _xis;
				XINPUT_STATE _lxis;
#endif
			public:
#ifdef WIN32
				state(const XINPUT_STATE& x, const XINPUT_STATE& l)
					: _xis(x), _lxis(l){}
#endif
				vec2 left_stick() const;
				vec2 delta_left_stick() const;
				vec2 right_stick() const;
				vec2 delta_right_stick() const;
				float left_trigger() const;
				float delta_left_trigger() const;
				float right_trigger() const;
				float delta_right_trigger() const;
				bool button_down(button b) const;
				bool button_pressed(button b) const;
				vec2 dpad_stick() const;
				vec2 delta_dpad_stick() const;
			};
			
			gamepad(uint idx);

			bool update();

			inline state get_state() const { return state(_cxis, _lxis); }
			
			void vibrate(vec2 vibe) const;

			static bool is_connected(uint idx);


			propr(bool, connected, { return _connected; });
			propr(uint, user_index, { return _usr_idx; });
		};
	}
}