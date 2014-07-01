#pragma once
#include "cmmn.h"
#include "render_target.h"

namespace qeg 
{
	class depth_render_texture2d : public texture2d, public render_target
	{
#ifdef DIRECTX
		ComPtr<ID3D11DepthStencilView> dsv;
#elif OPENGL
		GLuint _fbo;
#endif
		viewport _vp;
	public:
		depth_render_texture2d(device* _dev, uvec2 size
			/*, pixel_format df = pixel_format::D28_UNORM_S8_UINT or whatever when depth formats are supported*/);
		depth_render_texture2d(device* _dev, const viewport& vp
			/*, pixel_format df = pixel_format::D28_UNORM_S8_UINT or whatever when depth formats are supported*/);

		void ombind(device* _dev) override;
		inline viewport& mviewport() override { return _vp; }

		~depth_render_texture2d();

#ifdef DIRECTX
		depth_render_texture2d(uvec2 size_, ComPtr<ID3D11DepthStencilView> dsv_)
			: _vp(size_), dsv(dsv_){}
		propr(ComPtr<ID3D11DepthStencilView>, depth_stencil, { return dsv; });
#endif
#ifdef OPENGL
		depth_render_texture2d(uvec2 size_, GLuint db);
		propr(GLuint, frame_buffer, { return _fbo; });
		propr(GLuint, depth_buffer, { return _id; });
#endif
	};
}