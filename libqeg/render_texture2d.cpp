#include "render_texture2d.h"

namespace qeg
{
#ifdef DIRECTX
	render_texture2d::render_texture2d(device& dev, vec2 size_, pixel_format f)
		: texture2d(dev, CD3D11_TEXTURE2D_DESC((DXGI_FORMAT)f, size_.x, size_.y, 1, 1, 
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE))
	{
		CD3D11_TEXTURE2D_DESC dd(DXGI_FORMAT_R24G8_TYPELESS, size_.x, size_.y);
		dd.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		ComPtr<ID3D11Texture2D> dst;
		chr(dev.ddevice()->CreateTexture2D(&dd, nullptr, &dst));
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(dst.Get(), D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT);
		chr(dev.ddevice()->CreateDepthStencilView(dst.Get(), &dsvd, dsv.GetAddressOf()));
		CD3D11_RENDER_TARGET_VIEW_DESC rsvd(texd.Get(), D3D11_RTV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)f);
		chr(dev.ddevice()->CreateRenderTargetView(texd.Get(), &rsvd, &rtv));
	}


	render_texture2d::~render_texture2d()
	{
	}
#elif defined(OPENGL)
	render_texture2d::render_texture2d(device& dev, vec2 size, pixel_format f)
	{	
		glGenFramebuffers(1, &framebuf);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuf);
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, get_gl_format_internal(f), _size.x, _size.y, 0, (GLenum)f, get_gl_format_type(f), 0);
		glGenRenderbuffers(1, &depthbuf);
		glBindRenderbuffer(GL_RENDERBUFFER, depthbuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size.x, _size.y);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, depthbuf, 0);
		GLenum db[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, db);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	render_texture2d::~render_texture2d()
	{	
		if(depthbuf != 0)
			glDeleteRenderbuffers(1, &depthbuf);
		if (framebuf != 0)
			glDeleteFramebuffers(1, &framebuf);
	}
#endif
}