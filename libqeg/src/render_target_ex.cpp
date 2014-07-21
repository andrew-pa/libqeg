#include "render_target_ex.h"

namespace qeg
{
#ifdef DIRECTX
	depth_render_texture2d::depth_render_texture2d(device* _dev, uvec2 size, pixel_format df)
		: texture2d(_dev, CD3D11_TEXTURE2D_DESC((DXGI_FORMAT)detail::get_texture_format_for_depth(df)/*DXGI_FORMAT_R24G8_TYPELESS*/, size.x, size.y, 1, 1, 
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE)), _vp(size)
	{
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(texd.Get(), D3D11_DSV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)df /*DXGI_FORMAT_D24_UNORM_S8_UINT*/);
		chr(_dev->ddevice()->CreateDepthStencilView(texd.Get(), &dsvd, dsv.GetAddressOf()));
	}
	depth_render_texture2d::depth_render_texture2d(device* _dev, const viewport& vp, pixel_format df)
		: texture2d(_dev, CD3D11_TEXTURE2D_DESC((DXGI_FORMAT)detail::get_texture_format_for_depth(df)/*DXGI_FORMAT_R24G8_TYPELESS*/, vp.size.x, vp.size.y, 1, 1,
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE)), _vp(vp)
	{
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(texd.Get(), D3D11_DSV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)df/*DXGI_FORMAT_D24_UNORM_S8_UINT*/);
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

	render_textureCube::render_textureCube(device* _dev, uint size, pixel_format f, pixel_format df)
		: _vp(vec2(size)), textureCube(_dev, CD3D11_TEXTURE2D_DESC((DXGI_FORMAT)f, size, size, 6, 1,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE))
	{
		//create shared dsv
		CD3D11_TEXTURE2D_DESC dd(
			(DXGI_FORMAT)detail::get_texture_format_for_depth(df)/*DXGI_FORMAT_R24G8_TYPELESS*/, size, size);
		dd.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		ComPtr<ID3D11Texture2D> dst;
		chr(_dev->ddevice()->CreateTexture2D(&dd, nullptr, &dst));
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(dst.Get(), D3D11_DSV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)df);
		chr(_dev->ddevice()->CreateDepthStencilView(dst.Get(), &dsvd, dsv.GetAddressOf()));

		//create each face's resources
		CD3D11_RENDER_TARGET_VIEW_DESC rtvd(D3D11_RTV_DIMENSION_TEXTURE2DARRAY,
			(DXGI_FORMAT)f);
		rtvd.Texture2DArray.ArraySize = 1;
		rtvd.Texture2DArray.MipSlice = 0;
		for (int i = 0; i < 6; ++i)
		{
			rtx[i] = nullptr; 
			rtvd.Texture2DArray.FirstArraySlice = i;
			chr(_dev->ddevice()->CreateRenderTargetView(texd.Get(), &rtvd, &rtv[i]));
		}
	}

	render_textureCube::render_textureCube(device* _dev, const viewport& vp, pixel_format f, pixel_format df)
		: _vp(vp), textureCube(_dev, CD3D11_TEXTURE2D_DESC((DXGI_FORMAT)f, vp.size.x, vp.size.y, 6, 1,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE))
	{
		//create shared dsv
		CD3D11_TEXTURE2D_DESC dd((DXGI_FORMAT)detail::get_texture_format_for_depth(df)/*DXGI_FORMAT_R24G8_TYPELESS*/, vp.size.x, vp.size.y);
		dd.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		ComPtr<ID3D11Texture2D> dst;
		chr(_dev->ddevice()->CreateTexture2D(&dd, nullptr, &dst));
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(dst.Get(), D3D11_DSV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)df /*_D24_UNORM_S8_UINT*/);
		chr(_dev->ddevice()->CreateDepthStencilView(dst.Get(), &dsvd, dsv.GetAddressOf()));

		//create each face's resources
		CD3D11_RENDER_TARGET_VIEW_DESC rtvd(D3D11_RTV_DIMENSION_TEXTURE2DARRAY,
			(DXGI_FORMAT)f);
		rtvd.Texture2DArray.ArraySize = 1;
		rtvd.Texture2DArray.MipSlice = 0;
		for (int i = 0; i < 6; ++i)
		{
			rtx[i] = nullptr;
			rtvd.Texture2DArray.FirstArraySlice = i;
			chr(_dev->ddevice()->CreateRenderTargetView(texd.Get(), &rtvd, &rtv[i]));
		}
	}

	void render_textureCube::render_textureCube_face::ombind(device* _dev)	
	{
		//see also render_texture2d::ombind
		const float clear_color[] = { 1.f, .5f, 0.f, 0.f };
		if (rtc->rtv[idx] != nullptr)
			_dev->context()->ClearRenderTargetView(rtc->rtv[idx].Get(), clear_color);
		if (rtc->dsv != nullptr)
			_dev->context()->ClearDepthStencilView(rtc->dsv.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
		_dev->context()->OMSetRenderTargets(1, rtc->rtv[idx].GetAddressOf(), rtc->dsv.Get());
		CD3D11_VIEWPORT vp;
		vp.TopLeftX = rtc->_vp.offset.x;
		vp.TopLeftY = rtc->_vp.offset.y;
		vp.Width = rtc->_vp.size.x < 0 ? _dev->size().x : rtc->_vp.size.x;
		vp.Height = rtc->_vp.size.y < 0 ? _dev->size().y : rtc->_vp.size.y;
		vp.MinDepth = rtc->_vp.min_depth < 0 ? D3D11_MIN_DEPTH : rtc->_vp.min_depth;
		vp.MaxDepth = rtc->_vp.max_depth < 0 ? D3D11_MAX_DEPTH : rtc->_vp.max_depth;
		_dev->context()->RSSetViewports(1, &vp);
	}

	render_textureCube::~render_textureCube()
	{
	}
#elif OPENGL
	depth_render_texture2d::depth_render_texture2d(device* _dev, uvec2 size, pixel_format f /*depth pixel_formats needed!*/)
		: texture2d(size), _vp(size), _wstencil(has_stencil(f))
	{
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

		glGenTextures(1, &_id);

		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)f, _size.x, _size.y, 0,
			detail::get_gl_format_internal(f), detail::get_gl_format_type(f), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, _wstencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _id, 0);

		glDrawBuffer(GL_NONE);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	depth_render_texture2d::depth_render_texture2d(device* _dev, const viewport& vp, pixel_format f /*depth pixel_formats needed!*/)
		: texture2d(vp.size), _vp(vp), _wstencil(has_stencil(f))
	{
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

		glGenTextures(1, &_id);

		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)f, vp.size.x, vp.size.y, 0,
			detail::get_gl_format_internal(f), detail::get_gl_format_type(f), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, _wstencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _id, 0);

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
		if (_wstencil) glClear(GL_STENCIL_BITS);
	}

	depth_render_texture2d::~depth_render_texture2d()
	{
		if (_fbo != 0)
			glDeleteFramebuffers(1, &_fbo);
	}

	void render_textureCube::render_textureCube_face::ombind(device* _dev)
	{
		auto _vp = rtc->_vp;
		glBindFramebuffer(GL_FRAMEBUFFER, rtc->_fbo[idx]);
		glViewport(_vp.offset.x, _vp.offset.y,
			_vp.size.x < 0 ? _dev->size().x : _vp.size.x,
			_vp.size.y < 0 ? _dev->size().y : _vp.size.y);
		glDepthRange(_vp.min_depth < 0 ? 0.f : _vp.min_depth,
			_vp.max_depth < 0 ? 1.f : _vp.max_depth);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (rtc->_wstencil) glClear(GL_STENCIL_BITS);
	}

	render_textureCube::render_textureCube(device* _dev, uint size, pixel_format f, pixel_format df)
		: _vp(uvec2(size)), textureCube(_dev, uvec2(size), f, vector<byte*>({ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr })),
			_wstencil(has_stencil(df))
	{
		glGenFramebuffers(6, _fbo);

		glGenTextures(1, &_db);
		glBindTexture(GL_TEXTURE_2D, _db);
		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)df, _size.x, _size.y, 0,
			detail::get_gl_format_internal(df), detail::get_gl_format_type(df), 0);
	
		for (int i = 0; i < 6; ++i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _fbo[i]);
			glFramebufferTextureFaceEXT(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _id, 0, i+GL_TEXTURE_CUBE_MAP_POSITIVE_X);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, _wstencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _db, 0);
		}

		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	render_textureCube::render_textureCube(device* _dev, const viewport& vp, pixel_format f, pixel_format df)
		: _vp(vp), textureCube(_dev, vp.size, f, vector<byte*>({ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr })),
		_wstencil(has_stencil(df))
	{
		glGenFramebuffers(6, _fbo);

		glGenTextures(1, &_db);
		glBindTexture(GL_TEXTURE_2D, _db);
		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)f, _size.x, _size.y, 0,
			detail::get_gl_format_internal(f), detail::get_gl_format_type(f), 0);

		for (int i = 0; i < 6; ++i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _fbo[i]);
			glFramebufferTextureFaceEXT(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _id, 0, i + GL_TEXTURE_CUBE_MAP_POSITIVE_X);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, _wstencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _db, 0);
		}

		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	render_textureCube::~render_textureCube()
	{
		if (_db != 0)
			glDeleteTextures(1, &_db);
		if (_fbo[0] != 0)
			glDeleteFramebuffers(6, _fbo);
	}

#endif
}