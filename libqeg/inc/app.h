#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	class app
	{
	protected:
		device* _dev;
		float fps; float mpf;
		bool var_fps; float targ_mpf;

		string _title;
#if defined(WIN32) && defined(DIRECTX)
		HWND wnd;
#elif OPENGL
		GLFWwindow* wnd;
#endif
	public:
		app(const string& title, vec2 winsize, uint aa_samples, bool vfps = false, float tmpf = 1.f/70.f);

		~app();
		
		void run();

		virtual void render(float t, float dt) = 0;
		virtual void update(float t, float dt) = 0;
		virtual void resized(){}
	};
}

