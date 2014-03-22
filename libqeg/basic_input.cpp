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

		bool key_state[255], last_key_state[255];

		bool keyboard::state::key_down(key k) const
		{
			return key_state[(int)k];
		}

		bool keyboard::state::key_pressed(key k) const
		{
			return last_key_state[(int)k] && !key_state[(int)k];
		}

		void keyboard::__update(key kc, bool s)
		{
			last_key_state[(int)kc] = key_state[(int)kc];
			key_state[(int)kc] = s;
		}
	}
}
