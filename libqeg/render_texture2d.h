#pragma once
#include "cmmn.h"
#include "device.h"
#include "texture.h"

namespace qeg
{
	class render_texture2d : public qeg::texture2d
	{
	protected:
#ifdef DIRECTX
		ComPtr<ID3D11RenderTargetView> rtv;
		ComPtr<ID3D11DepthStencilView> dsv;
#endif
#ifdef OPENGL
		GLuint framebuf;
		GLuint depthbuf;
#endif
	public:
		render_texture2d(device* dev, uvec2 size, buffer_format f = buffer_format::RGBA32_FLOAT);
		~render_texture2d();

#ifdef DIRECTX
		render_texture2d(uvec2 size_, ComPtr<ID3D11RenderTargetView> rtv_, ComPtr<ID3D11DepthStencilView> dsv_)
			: texture2d(size_), rtv(rtv_), dsv(dsv_) {}
		propr(ComPtr<ID3D11RenderTargetView>, render_target, { return rtv; });
		propr(ComPtr<ID3D11DepthStencilView>, depth_stencil, { return dsv; });
#endif
#ifdef OPENGL
		render_texture2d(uvec2 size_, GLuint fb, GLuint db)
			: texture2d(size_), framebuf(fb), depthbuf(db) {}
		propr(GLuint, frame_buffer, { return framebuf; });
		propr(GLuint, depth_buffer, { return depthbuf; });
#endif
	};
};

