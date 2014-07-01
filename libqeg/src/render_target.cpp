#include "render_target.h"

namespace qeg
{
#ifdef DIRECTX
	render_texture2d::render_texture2d(device* dev, uvec2 size_, pixel_format f)
		: texture2d(dev, CD3D11_TEXTURE2D_DESC((DXGI_FORMAT)f, size_.x, size_.y, 1, 1, 
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)), _vp(size_)
	{
		CD3D11_TEXTURE2D_DESC dd(DXGI_FORMAT_R24G8_TYPELESS, size_.x, size_.y);
		dd.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		ComPtr<ID3D11Texture2D> dst;
		chr(dev->ddevice()->CreateTexture2D(&dd, nullptr, &dst));
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(dst.Get(), D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT);
		chr(dev->ddevice()->CreateDepthStencilView(dst.Get(), &dsvd, dsv.GetAddressOf()));
		CD3D11_RENDER_TARGET_VIEW_DESC rsvd(texd.Get(), D3D11_RTV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)f);
		chr(dev->ddevice()->CreateRenderTargetView(texd.Get(), &rsvd, &rtv));
	}

	render_texture2d::render_texture2d(device* dev, const viewport& vp, pixel_format f)
		: texture2d(dev, CD3D11_TEXTURE2D_DESC((DXGI_FORMAT)f, vp.size.x, vp.size.y, 1, 1,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)), _vp(vp)
	{
		CD3D11_TEXTURE2D_DESC dd(DXGI_FORMAT_R24G8_TYPELESS, _vp.size.x, _vp.size.y);
		dd.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		ComPtr<ID3D11Texture2D> dst;
		chr(dev->ddevice()->CreateTexture2D(&dd, nullptr, &dst));
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(dst.Get(), D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT);
		chr(dev->ddevice()->CreateDepthStencilView(dst.Get(), &dsvd, dsv.GetAddressOf()));
		CD3D11_RENDER_TARGET_VIEW_DESC rsvd(texd.Get(), D3D11_RTV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)f);
		chr(dev->ddevice()->CreateRenderTargetView(texd.Get(), &rsvd, &rtv));
	}

	void render_texture2d::ombind(device* _dev)
	{
		const float clear_color[] = { 1.f, .5f, 0.f, 0.f };
		if (rtv != nullptr)
			_dev->context()->ClearRenderTargetView(rtv.Get(), clear_color);
		if (dsv != nullptr)
			_dev->context()->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
		_dev->context()->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());
		CD3D11_VIEWPORT vp;
		vp.TopLeftX = _vp.offset.x;
		vp.TopLeftY = _vp.offset.y;
		vp.Width = _vp.size.x < 0 ? _dev->size().x : _vp.size.x;
		vp.Height = _vp.size.y < 0 ? _dev->size().y : _vp.size.y;
		vp.MinDepth = _vp.min_depth < 0 ? D3D11_MIN_DEPTH : _vp.min_depth;
		vp.MaxDepth = _vp.max_depth < 0 ? D3D11_MAX_DEPTH : _vp.max_depth;
		_dev->context()->RSSetViewports(1, &vp);
	}

	void default_render_target::ombind(device* _dev)
	{
		_dev->_back_buffer->ombind(_dev);
	}

	render_texture2d::~render_texture2d()
	{
		if (dsv != nullptr) 
		{ 
			dsv.Reset(); dsv = nullptr; 
		}
		if (rtv != nullptr) 
		{
			rtv.Reset(); rtv = nullptr; 
		}
	}
#elif defined(OPENGL)
	render_texture2d::render_texture2d(device* dev, uvec2 size, pixel_format f)
		: texture2d(size), _vp(size)
	{	

		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

		glGenTextures(1, &_id);
		glGenTextures(1, &_db);

		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)f, _size.x, _size.y, 0, 
			detail::get_gl_format_internal(f), detail::get_gl_format_type(f), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _id, 0);

		glBindTexture(GL_TEXTURE_2D, _db);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _size.x, _size.y, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _db, 0);

		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	render_texture2d::render_texture2d(device* dev, const viewport& vp, pixel_format f)
		: texture2d(vp.size), _vp(vp)
	{	

		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

		glGenTextures(1, &_id);
		glGenTextures(1, &_db);

		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)f, _size.x, _size.y, 0, 
			detail::get_gl_format_internal(f), detail::get_gl_format_type(f), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _id, 0);

		glBindTexture(GL_TEXTURE_2D, _db);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _size.x, _size.y, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _db, 0);

		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void render_texture2d::ombind(device* _dev)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		glViewport(_vp.offset.x, _vp.offset.y, 
			_vp.size.x < 0 ? _dev->size().x : _vp.size.x, 
			_vp.size.y < 0 ? _dev->size().y : _vp.size.y);
		glDepthRange(_vp.min_depth < 0 ? 0.f : _vp.min_depth, 
			_vp.max_depth < 0 ? 1.f : _vp.max_depth);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void default_render_target::ombind(device* _dev)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, _dev->size().x, _dev->size().y);
		glDepthRange(0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	render_texture2d::~render_texture2d()
	{	
		if (_db != 0)
			glDeleteTextures(1, &_db);
		if (_fbo != 0)
			glDeleteFramebuffers(1, &_fbo);
	}
#endif
}