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
	
	class render_textureCube : public textureCube
	{
		//1x DSV for all faces
		//1x viewport for all faces
		//DX: 6x RSVs for each face 
		//GL: 6x FBOs for each face
		viewport _vp;

		struct render_textureCube_face : public render_target
		{
			render_textureCube* rtc;
			uint idx;
			render_textureCube_face(render_textureCube* rtc_, uint _i)
				: rtc(rtc_), idx(_i){}

			void ombind(device* _dev) override;

			viewport& mviewport() override
			{
				return rtc->_vp;
			}
		};

		render_textureCube_face* rtx[6];

#ifdef DIRECTX
		ComPtr<ID3D11RenderTargetView> rtv[6];
		ComPtr<ID3D11DepthStencilView> dsv;
#elif OPENGL
		GLuint _fbo[6];
		GLuint _db;
#endif
	public:
		render_textureCube(device* _dev, uint size, pixel_format f = pixel_format::RGBA32_FLOAT);
		render_textureCube(device* _dev, const viewport& vp, pixel_format f = pixel_format::RGBA32_FLOAT);
		~render_textureCube();

		inline render_target* target_for_face(uint idx)
		{
			if (rtx[idx] == nullptr)
			{
				rtx[idx] = new render_textureCube_face(this, idx);
			}
			return rtx[idx];
		}

		
		proprw(viewport, mviewport, { return _vp; })
	};
}