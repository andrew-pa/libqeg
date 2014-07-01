#include "render_texture_ex.h"

namespace qeg
{
#ifdef DIRECTX
	depth_render_texture2d::depth_render_texture2d(device* _dev, uvec2 size)
		: texture2d(_dev, CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R24G8_TYPELESS, size.x, size.y, 1, 1, 
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE)), _vp(size)
	{
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(texd.Get(), D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT);
		chr(_dev->ddevice()->CreateDepthStencilView(texd.Get(), &dsvd, dsv.GetAddressOf()));
	}
	depth_render_texture2d::depth_render_texture2d(device* _dev, const viewport& vp)
		: texture2d(_dev, CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R24G8_TYPELESS, vp.size.x, vp.size.y, 1, 1,
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE)), _vp(vp)
	{
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(texd.Get(), D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT);
		chr(_dev->ddevice()->CreateDepthStencilView(texd.Get(), &dsvd, dsv.GetAddressOf()));
	}

	void depth_render_texture2d::ombind(device* _dev)
	{
		if (dsv != nullptr)
			_dev->context()->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
		ID3D11RenderTargetView* nullviews[] = { nullptr };
		_dev->context()->OMSetRenderTargets(1, nullviews, dsv.Get());
		CD3D11_VIEWPORT vp;
		vp.TopLeftX = _vp.offset.x;
		vp.TopLeftY = _vp.offset.y;
		vp.Width = _vp.size.x < 0 ? _dev->size().x : _vp.size.x;
		vp.Height = _vp.size.y < 0 ? _dev->size().y : _vp.size.y;
		vp.MinDepth = _vp.min_depth < 0 ? D3D11_MIN_DEPTH : _vp.min_depth;
		vp.MaxDepth = _vp.max_depth < 0 ? D3D11_MAX_DEPTH : _vp.max_depth;
		_dev->context()->RSSetViewports(1, &vp);
	}

	depth_render_texture2d::~depth_render_texture2d()
	{
		if(dsv != nullptr)
		{
			dsv.Reset(); dsv = nullptr;
		}
	}
#elif OPENGL
	depth_render_texture2d::depth_render_texture2d(device* _dev, uvec2 size /*depth pixel_formats needed!*/)
		: texture2d(size), _vp(size)
	{
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

		glGenTextures(1, &_id);

		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _size.x, _size.y, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _id, 0);

		glDrawBuffer(GL_NONE);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	depth_render_texture2d::depth_render_texture2d(device* _dev, const viewport& vp /*depth pixel_formats needed!*/)
		: texture2d(vp.size), _vp(vp)
	{
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

		glGenTextures(1, &_id);

		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, vp.size.x, vp.size.y, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _id, 0);

		glDrawBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void depth_render_texture2d::ombind(device* _dev)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		glViewport(_vp.offset.x, _vp.offset.y,
			_vp.size.x < 0 ? _dev->size().x : _vp.size.x,
			_vp.size.y < 0 ? _dev->size().y : _vp.size.y);
		glDepthRange(_vp.min_depth < 0 ? 0.f : _vp.min_depth,
			_vp.max_depth < 0 ? 1.f : _vp.max_depth);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	depth_render_texture2d::~depth_render_texture2d()
	{
		if (_fbo != 0)
			glDeleteFramebuffers(1, &_fbo);
	}
#endif
}