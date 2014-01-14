#define LINK
#include <cmmn.h>
#include <device.h>
#include <app.h>
using namespace qeg;

class qegtest_app : public app
{
public:
	qegtest_app()
		: app(L"libqeg test", vec2(640, 480), true)
	{
	}
	void update(float t, float dt) override
	{
	}
	
	void resized() override
	{
	}

	void render(float t, float dt) override
	{
	}
};

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR cmdLine, int cmdShow)
{
	qegtest_app a;
	a.run();
}