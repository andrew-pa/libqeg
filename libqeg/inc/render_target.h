#pragma once
#include "cmmn.h"
#include "device.h"
#include "texture.h"

namespace qeg
{
	struct viewport
	{
		vec2 offset;
		vec2 size;
		float min_depth;
		float max_depth;
		viewport(vec2 si, vec2 os = vec2(0.), float md = -1.f, float xd = -1.f)
			: offset(os), size(si), min_depth(md), max_depth(xd){}
	};

	class render_target 
	{
	public:
		virtual void ombind(device* _dev) = 0;
		virtual viewport& mviewport() = 0;
	};
	
	class default_render_target : public render_target
	{
		viewport vp;
	public:
		default_render_target(const viewport& _vp = viewport(vec2(-1))) //viewport(vec2(-1)) makes a viewport that renders to the whole screen with the default settings
			: vp(_vp) {}
		void ombind(device* _dev) override;
		inline viewport& mviewport() override { return vp; }
	};

	class render_texture2d : public qeg::texture2d, public render_target
	{
	protected:
#ifdef DIRECTX
		ComPtr<ID3D11RenderTargetView> rtv;
		ComPtr<ID3D11DepthStencilView> dsv;
#endif
#ifdef OPENGL
		GLuint _fbo;
		GLuint _db;
#endif
		viewport _vp;
	public:
		render_texture2d(device* dev, uvec2 size, pixel_format f = pixel_format::RGBA32_FLOAT);
		render_texture2d(device* dev, const viewport& vp, pixel_format f = pixel_format::RGBA32_FLOAT);
		~render_texture2d();

		void ombind(device* _dev) override;
		inline viewport& mviewport() override { return _vp; }

#ifdef DIRECTX
		render_texture2d(uvec2 size_, ComPtr<ID3D11RenderTargetView> rtv_, ComPtr<ID3D11DepthStencilView> dsv_)
			: texture2d(size_), rtv(rtv_), dsv(dsv_), _vp(size_) {}
		//render_texture2d(const viewport& vp_, ComPtr<ID3D11RenderTargetView> rtv_, ComPtr<ID3D11DepthStencilView> dsv_)
		//	: texture2d(vp_.size), rtv(rtv_), dsv(dsv_), _vp(vp_) {}
		propr(ComPtr<ID3D11RenderTargetView>, render_target, { return rtv; });
		propr(ComPtr<ID3D11DepthStencilView>, depth_stencil, { return dsv; });
#endif
#ifdef OPENGL
		render_texture2d(uvec2 size_, GLuint fb, GLuint db)
			: texture2d(size_), _fbo(fb), _db(db), _vp(size_) {}
		propr(GLuint, frame_buffer, { return _fbo; });
		propr(GLuint, depth_buffer, { return _db; });
#endif
	};
};

