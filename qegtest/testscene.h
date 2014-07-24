#pragma once
class qegtests_app;

class testscene
{
public:
	virtual void reset() = 0;
	virtual void update(float t, float dt) = 0;
	virtual void render(float t, float dt) = 0;
	virtual void resize() = 0;
	virtual ~testscene() {}
};