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

		void mouse::__update(vec2 p, bool left, bool middle, bool right)
		{
			mouse_lst = mouse_cst;
			mouse_cst.pos = p;
			mouse_cst.left = left;
			mouse_cst.middle = middle;
			mouse_cst.right = right;
		}

		bool key_state[160], last_key_state[160];

		bool keyboard::state::key_down(key k) const
		{
			return key_state[(int)k];
		}

		bool keyboard::state::key_pressed(key k) const
		{
			return !last_key_state[(int)k] && key_state[(int)k];
		}

		void keyboard::__update(key kc, bool s)
		{
			last_key_state[(int)kc] = key_state[(int)kc];
			key_state[(int)kc] = s;
		}


		gamepad::gamepad(uint idx)
#ifdef WIN32
			: _usr_idx(idx), _connected(false){}
#endif

		void gamepad::update(bool tnc)
#ifdef WIN32
		{
			auto err = XInputGetState(_usr_idx, &_cxis);
			if(err == ERROR_DEVICE_NOT_CONNECTED)
			{
				_connected = false;
				if(tnc) throw gamepad_not_connected_exception();
			}
			else if(err != ERROR_SUCCESS)
			{
				ostringstream oss;
				oss << "Gamepad " << _usr_idx << " error:" << err;
				throw error_code_exception(err, oss.str());
			}
			_connected = true;
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

		vec2 gamepad::state::right_stick() const
#ifdef WIN32
		{
			return vec2(
				dead_zone(_xis.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE),
				dead_zone(_xis.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
		}
#endif

		float gamepad::state::left_trigger() const
#ifdef WIN32
		{
			return dead_zone(_xis.Gamepad.bLeftTrigger,
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

		bool gamepad::state::is_button_down(button b) const
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

		vec2 gamepad::state::dpad_stick() const
		{
			vec2 res = vec2(0);
			
			if (is_button_down(button::dpad_up))
				res.y = 1;
			else if (is_button_down(button::dpad_down))
				res.y = -1;
			if (is_button_down(button::dpad_left))
				res.x = -1;
			else if (is_button_down(button::dpad_right))
				res.x = 1;


			return res;
		}

	}
}
