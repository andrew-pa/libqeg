#include "texture2d.h"

namespace qeg
{
#ifdef DIRECTX
	texture2d::texture2d(device& dev, vec2 size_, pixel_format f, void* data, bool gen_mips, size_t sys_pitch)
		: _size(size_)
	{
		CD3D11_TEXTURE2D_DESC txd((DXGI_FORMAT)f, size_.x, size_.y, 1U, 0U,
			D3D11_BIND_SHADER_RESOURCE);
		CD3D11_SHADER_RESOURCE_VIEW_DESC srd(D3D11_SRV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)f);
		D3D11_SUBRESOURCE_DATA initdata = { 0 };
		initdata.pSysMem = data;
		initdata.SysMemPitch = sys_pitch;
		chr(dev.ddevice()->CreateTexture2D(&txd, (data == nullptr ? nullptr : &initdata), &texd));
		chr(dev.ddevice()->CreateShaderResourceView(texd.Get(), &srd, &srv));
	}

	texture2d::texture2d(device& dev, CD3D11_TEXTURE2D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc)
		: _size(desc.Width, desc.Height)
	{
		chr(dev.ddevice()->CreateTexture2D(&desc, nullptr, &texd));
		chr(dev.ddevice()->CreateShaderResourceView(texd.Get(), &srvdesc, &srv));
	}
	texture2d::texture2d(device& dev, CD3D11_TEXTURE2D_DESC desc)
		: _size(desc.Width, desc.Height)
	{
		chr(dev.ddevice()->CreateTexture2D(&desc, nullptr, &texd));
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc(texd.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, desc.Format);
		chr(dev.ddevice()->CreateShaderResourceView(texd.Get(), &srvdesc, &srv));
	}

	void texture2d::bind(device& dev, int slot)
	{
		dev.context()->PSSetShaderResources(slot, 1, srv.GetAddressOf());
	}

	void texture2d::unbind(device& dev, int slot)
	{
		dev.context()->PSSetShaderResources(slot, 1, nullptr);
	}


	texture2d::~texture2d(){}
#endif

#ifdef OPENGL
	texture2d::texture2d(device& dev, vec2 size_, pixel_format f, void* data, bool gen_mips)
		: _size(size_)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, get_gl_format_internal(f), 
			size_.x, size_.y, 0, (GLenum)f, get_gl_format_type(f), data);
		if (gen_mips)
			glGenerateMipmap(GL_TEXTURE_2D);
	}

	void texture2d::bind(device& dev, int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, _id);
	}

	void texture2d::unbind(device& dev, int slot)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	texture2d::~texture2d()
	{
		if (_id != 0)
			glDeleteTextures(1, &_id);
	}
#endif
}