#define LINK
#include <cmmn.h>
#include <device.h>
#include <app.h>
using namespace qeg;

class qegtest_app : public app
{
	//ComPtr<ID2D1SolidColorBrush> brush;
public:
	qegtest_app()
		: app(L"libqeg test", vec2(640, 480), true)
	{
		//_dev->d2context()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GreenYellow), &brush);
	}

	void update(float t, float dt) override
	{
	}
	
	void resized() override
	{
	}

	void render(float t, float dt) override
	{
		//_dev->d2context()->BeginDraw();
		//_dev->d2context()->DrawRectangle(D2D1::RectF(50, 50, 100, 100), brush.Get(), 5);
		//_dev->d2context()->EndDraw();
	}
};

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR cmdLine, int cmdShow)
{
	qegtest_app a;
	a.run();
}