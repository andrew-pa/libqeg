#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	class app
	{
	protected:
		device _dev;
		float fps; float mpf;
		float var_fps; float targ_mpf;

#ifdef WIN32
		HWND wnd;
#endif
	public:
		app(const wstring& title, vec2 winsize, bool vfps = false, float tmpf = 1.f / 100.f);
		
		void run();

		virtual void render(float t, float dt) = 0;
		virtual void update(float t, float dt) = 0;
		virtual void resized()
		{
			_dev.resize();
		}
	};
}

