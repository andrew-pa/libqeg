#include "render_texture_ex.h"

namespace qeg
{
#ifdef DIRECTX
	depth_render_texture2d::depth_render_texture2d(device* _dev, uvec2 size)
		: render_texture2d(size, nullptr, nullptr)
	{

	}
#elif OPENGL
#endif
}