#pragma once
#include "cmmn.h"
#include "render_texture2d.h"

namespace qeg 
{
	class depth_render_texture2d : public render_texture2d
	{
	public:
		depth_render_texture2d(device* _dev, uvec2 size
			/*, pixel_format df = pixel_format::D28_UNORM_S8_UINT or whatever when depth formats are supported*/);
#ifdef DIRECTX
		depth_render_texture2d(uvec2 size_, ComPtr<ID3D11DepthStencilView> dsv_)
			: render_texture2d(size_, nullptr, dsv_) {}
#endif
#ifdef OPENGL
		depth_render_texture2d(uvec2 size_, GLuint db)
			: render_texture2d(size_, -1, db) {}
#endif
	};
}