#include "texture.h"

#ifdef DIRECTX
#include "dds_loader.h"
#elif OPENGL
#include <SOIL.h>
#endif

namespace qeg
{
#ifdef DIRECTX
	texture1d::texture1d(device* dev, uint size_, buffer_format f, void* data, bool gen_mips, size_t sys_pitch)
	{
		CD3D11_TEXTURE1D_DESC dsc((DXGI_FORMAT)f, size_, 1U, 0U, D3D11_BIND_SHADER_RESOURCE);
		CD3D11_SHADER_RESOURCE_VIEW_DESC sd(D3D11_SRV_DIMENSION_TEXTURE1D, (DXGI_FORMAT)f);
		D3D11_SUBRESOURCE_DATA d = { 0 };
		d.pSysMem = data;
		d.SysMemPitch = sys_pitch;
		chr(dev->ddevice()->CreateTexture1D(&dsc, (data == nullptr ? nullptr : &d), &texd));
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &sd, &srv));
	}
	texture1d::texture1d(device* dev, CD3D11_TEXTURE1D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc)
		: texture(desc.Width)
	{
		chr(dev->ddevice()->CreateTexture1D(&desc, nullptr, &texd));
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srvdesc, &srv));
	}
	texture1d::texture1d(device* dev, CD3D11_TEXTURE1D_DESC desc)
		: texture(desc.Width)
	{
		chr(dev->ddevice()->CreateTexture1D(&desc, nullptr, &texd));
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc(texd.Get(), D3D11_SRV_DIMENSION_TEXTURE1D, desc.Format);
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srvdesc, &srv));
	}
	texture1d::texture1d(ComPtr<ID3D11ShaderResourceView> srv_)
		: texture(srv_)
	{
		ComPtr<ID3D11Resource> rs;
		srv->GetResource(&rs);
		rs.As(&texd);
	}

	texture2d::texture2d(device* dev, uvec2 size_, buffer_format f, void* data, bool gen_mips, size_t sys_pitch)
		: texture(size_)
	{
		CD3D11_TEXTURE2D_DESC txd((DXGI_FORMAT)f, size_.x, size_.y, 1U, 0U,
			D3D11_BIND_SHADER_RESOURCE);
		CD3D11_SHADER_RESOURCE_VIEW_DESC srd(D3D11_SRV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)f);
		D3D11_SUBRESOURCE_DATA initdata = { 0 };
		initdata.pSysMem = data;
		initdata.SysMemPitch = sys_pitch;
		chr(dev->ddevice()->CreateTexture2D(&txd, (data == nullptr ? nullptr : &initdata), &texd));
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srd, &srv));
	}

	texture2d::texture2d(device* dev, CD3D11_TEXTURE2D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc)
		: texture(uvec2(desc.Width, desc.Height))
	{
		chr(dev->ddevice()->CreateTexture2D(&desc, nullptr, &texd));
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srvdesc, &srv));
	}
	texture2d::texture2d(device* dev, CD3D11_TEXTURE2D_DESC desc)
		: texture(uvec2(desc.Width, desc.Height))
	{
		chr(dev->ddevice()->CreateTexture2D(&desc, nullptr, &texd));
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc(texd.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, desc.Format);
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srvdesc, &srv));
	}


	texture2d::texture2d(ComPtr<ID3D11ShaderResourceView> srv_)
		: texture(srv_)
	{
		ComPtr<ID3D11Resource> rs;
		srv->GetResource(&rs);
		rs.As(&texd);
	}
#endif

#ifdef OPENGL
	texture2d::texture2d(device* dev, uvec2 size_, buffer_format f, void* data, bool gen_mips)
		: texture(size_)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, get_gl_format_internal(f), 
			size_.x, size_.y, 0, (GLenum)f, get_gl_format_type(f), data);
		if (gen_mips)
			glGenerateMipmap(GL_TEXTURE_2D);
	}		
#endif

#ifdef DIRECTX
	texture2d* texture2d::load_dds(device* dev, datablob<byte>* data)
	{
		ID3D11ShaderResourceView* tv;
		qeg::dds_loader::CreateDDSTextureFromMemory(dev->ddevice().Get(), data->data, data->length, nullptr, &tv, 2048);
		return new texture2d(tv);
	}
#elif OPENGL
	texture2d* texture2d::load_dds(device* dev, datablob<byte>* data)
	{
		int width, height, chn;
		unsigned char* image =
			SOIL_load_image_from_memory(data->data, data->length, &width, &height, &chn, SOIL_LOAD_RGBA);//SOIL_load_image("img.png", &width, &height, 0, SOIL_LOAD_RGB);
		GLuint ti;
		glGenTextures(1, &ti);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ti);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		return new texture2d(vec2(width, height), ti);
	}
#endif
}