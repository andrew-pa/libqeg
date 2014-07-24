#include "basic_input.h"

namespace qeg 
{ 
	namespace input 
	{
		mouse::state mouse_cst, mouse_lst;

		const mouse::state mouse::get_state()
		{
			return mouse_cst;
		}

		const mouse::state mouse::get_delta_state()
		{
			return mouse::state(mouse_cst.pos - mouse_lst.pos, 
				!mouse_lst.left   && mouse_cst.left, 
				!mouse_lst.middle && mouse_cst.middle, 
				!mouse_lst.right  && mouse_cst.right);
		}

//		void mouse::toggle_cursor()
//		{
//#if defined(WIN32) && defined(DIRECTX)
//			static bool last_value = true;
//			ShowCursor(last_value = !last_value);
//#elif OPENGL
//			auto m = glfwGetInputMode(wnd, GLFW_CURSOR);
//			
//#endif
//		}

		void mouse::__update(vec2 p, bool left, bool middle, bool right)
		{
			mouse_lst = mouse_cst;
			mouse_cst.pos = p;
			mouse_cst.left = left;
			mouse_cst.middle = middle;
			mouse_cst.right = right;
		}

		bool key_state[160], last_key_state[160];
		mod_key mod_state;

		bool keyboard::state::key_down(key k) const
		{
			return key_state[(int)k];
		}

		bool keyboard::state::key_pressed(key k) const
		{
			return !last_key_state[(int)k] && key_state[(int)k];
		}

		mod_key keyboard::state::mods() const
		{
			return mod_state;
		}

		void keyboard::__update(key kc, bool s)
		{
			if (kc == key::control) 
				if (s) 
				{
					mod_state = (mod_key)((byte)mod_state | (byte)mod_key::control);
				}
				else 
				{
					mod_state = (mod_key)((byte)mod_state ^ (byte)mod_key::control); 
				}
			if (kc == key::shift)
				if (s)
				{
					mod_state = (mod_key)((byte)mod_state | (byte)mod_key::shift);
				}
				else
				{
					mod_state = (mod_key)((byte)mod_state ^ (byte)mod_key::shift);
				}
			if (kc == key::menu)
				if (s)
				{
					mod_state = (mod_key)((byte)mod_state | (byte)mod_key::alt);
				}
				else
				{
					mod_state = (mod_key)((byte)mod_state ^ (byte)mod_key::alt);
				}
			last_key_state[(int)kc] = key_state[(int)kc];
			key_state[(int)kc] = s;
		}


		gamepad::gamepad(uint idx)
#ifdef WIN32
			: _usr_idx(idx), _connected(false){}
#endif

		bool gamepad::update()
#ifdef WIN32
		{
			_lxis = _cxis;
			auto err = XInputGetState(_usr_idx, &_cxis);
			if(err == ERROR_DEVICE_NOT_CONNECTED)
			{
				_connected = false;
				return false;
			}
			else if(err != ERROR_SUCCESS)
			{
				ostringstream oss;
				oss << "Gamepad " << _usr_idx << " error:" << err;
				throw error_code_exception(err, oss.str());
			}
			_connected = true;
			return true;
		}
#endif

		void gamepad::vibrate(vec2 vibe) const
#ifdef WIN32
		{
			XINPUT_VIBRATION vi;
			vi.wLeftMotorSpeed = (WORD)(vibe.x * (float)0xffff);
			vi.wRightMotorSpeed = (WORD)(vibe.y * (float)0xffff);
			auto err = XInputSetState(_usr_idx, &vi);
			if(err != ERROR_SUCCESS)
			{
				if (err == ERROR_DEVICE_NOT_CONNECTED)
					throw gamepad_not_connected_exception();
				else
				{
					ostringstream oss;
					oss << "Gamepad " << _usr_idx << " error:" << err;
					throw error_code_exception(err, oss.str());
				}
			}
		}
#endif

		bool gamepad::is_connected(uint idx)
#ifdef WIN32
		{
			XINPUT_STATE x;
			auto err = XInputGetState(idx, &x);
			if (err == ERROR_DEVICE_NOT_CONNECTED)
			{
				return false;
			}
			else if (err != ERROR_SUCCESS)
			{
				ostringstream oss;
				oss << "Gamepad " << idx << " error:" << err;
				throw error_code_exception(err, oss.str());
			}
			return true;
		}
#endif

		vec2 gamepad::state::left_stick() const
#ifdef WIN32
		{
			return vec2(
				dead_zone(_xis.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
				dead_zone(_xis.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
		}
#endif
		vec2 gamepad::state::delta_left_stick() const
#ifdef WIN32
		{
			return left_stick() - vec2(
				dead_zone(_lxis.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
				dead_zone(_lxis.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
		}
#endif

		vec2 gamepad::state::right_stick() const
#ifdef WIN32
		{
			return vec2(
				dead_zone(_xis.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE),
				dead_zone(_xis.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
		}
#endif
		vec2 gamepad::state::delta_right_stick() const
#ifdef WIN32
		{
			return right_stick() - vec2(
				dead_zone(_lxis.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE),
				dead_zone(_lxis.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
		}
#endif

		float gamepad::state::left_trigger() const
#ifdef WIN32
		{
			return dead_zone(_xis.Gamepad.bLeftTrigger,
				XINPUT_GAMEPAD_TRIGGER_THRESHOLD, numeric_limits<byte>::max());
		}
#endif
		float gamepad::state::delta_left_trigger() const
#ifdef WIN32
		{
			return left_trigger() - dead_zone(_xis.Gamepad.bLeftTrigger,
				XINPUT_GAMEPAD_TRIGGER_THRESHOLD, numeric_limits<byte>::max());
		}
#endif

		float gamepad::state::right_trigger() const
#ifdef WIN32
		{
			return dead_zone(_xis.Gamepad.bRightTrigger,
				XINPUT_GAMEPAD_TRIGGER_THRESHOLD, numeric_limits<byte>::max());
		}
#endif
		float gamepad::state::delta_right_trigger() const
#ifdef WIN32
		{
			return right_trigger() - dead_zone(_xis.Gamepad.bRightTrigger,
				XINPUT_GAMEPAD_TRIGGER_THRESHOLD, numeric_limits<byte>::max());
		}
#endif

		bool gamepad::state::button_down(button b) const
#ifdef WIN32
		{
			switch (b)
			{
			case qeg::input::gamepad::button::A:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
			
			case qeg::input::gamepad::button::B:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;

			case qeg::input::gamepad::button::X:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;

			case qeg::input::gamepad::button::Y:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;

			case qeg::input::gamepad::button::start:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;

			case qeg::input::gamepad::button::back:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;

			case qeg::input::gamepad::button::home:
				return (_xis.Gamepad.wButtons & 0x4000) != 0; //hack, xbox guide button

			case qeg::input::gamepad::button::left_bumper:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;

			case qeg::input::gamepad::button::right_bumper:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;

			case qeg::input::gamepad::button::left_stick:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;

			case qeg::input::gamepad::button::right_stick:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;

			case qeg::input::gamepad::button::dpad_left:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;

			case qeg::input::gamepad::button::dpad_right:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

			case qeg::input::gamepad::button::dpad_up:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;

			case qeg::input::gamepad::button::dpad_down:
				return (_xis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;

			default:
				throw exception("invalid gamepad button");
			}
		}
#endif
		bool gamepad::state::button_pressed(button b) const
#ifdef WIN32
		{
			bool cu = button_down(b);
			switch (b)
			{
			case qeg::input::gamepad::button::A:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_A) == 0;

			case qeg::input::gamepad::button::B:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_B) == 0;

			case qeg::input::gamepad::button::X:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_X) == 0;

			case qeg::input::gamepad::button::Y:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_Y) == 0;

			case qeg::input::gamepad::button::start:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_START) == 0;

			case qeg::input::gamepad::button::back:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) == 0;

			case qeg::input::gamepad::button::home:
				return cu && (_lxis.Gamepad.wButtons & 0x4000) == 0; //hack, xbox guide button

			case qeg::input::gamepad::button::left_bumper:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) == 0;

			case qeg::input::gamepad::button::right_bumper:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) == 0;

			case qeg::input::gamepad::button::left_stick:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) == 0;

			case qeg::input::gamepad::button::right_stick:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) == 0;

			case qeg::input::gamepad::button::dpad_left:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) == 0;

			case qeg::input::gamepad::button::dpad_right:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) == 0;

			case qeg::input::gamepad::button::dpad_up:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) == 0;

			case qeg::input::gamepad::button::dpad_down:
				return cu && (_lxis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) == 0;

			default:
				throw exception("invalid gamepad button");
			}
		}
#endif
		

		vec2 gamepad::state::dpad_stick() const
		{
			vec2 res = vec2(0);
			
			if (button_down(button::dpad_up))
				res.y = 1;
			else if (button_down(button::dpad_down))
				res.y = -1;
			if (button_down(button::dpad_left))
				res.x = -1;
			else if (button_down(button::dpad_right))
				res.x = 1;


			return res;
		}

		vec2 gamepad::state::delta_dpad_stick() const
		{
			vec2 res = vec2(0);

			if (button_down(button::dpad_up))
				res.y = 1;
			else if (button_down(button::dpad_down))
				res.y = -1;
			if (button_down(button::dpad_left))
				res.x = -1;
			else if (button_down(button::dpad_right))
				res.x = 1;


			return dpad_stick() - res;
		}
	}
}
