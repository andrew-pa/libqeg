#include "texture2d.h"

#ifdef DIRECTX
#include "dds_loader.h"
#elif OPENGL
#include <SOIL.h>
//#include <gli/gli.hpp>
#endif

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


	texture2d::texture2d(ComPtr<ID3D11ShaderResourceView> srv_)
		: srv(srv_)
	{
		ComPtr<ID3D11Resource> rs;
		srv->GetResource(&rs);
		rs.As(&texd);
	}

	void texture2d::bind(device& dev, int slot, shader& s)
	{
		dev.context()->PSSetShaderResources(slot, 1, srv.GetAddressOf());
	}

	void texture2d::unbind(device& dev, int slot, shader& s)
	{
		dev.context()->PSSetShaderResources(slot, 1, nullptr);
	}


	texture2d::~texture2d(){}
#endif

#ifdef OPENGL
	texture2d::texture2d(device* dev, vec2 size_, pixel_format f, void* data, bool gen_mips)
		: _size(size_)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, get_gl_format_internal(f), 
			size_.x, size_.y, 0, (GLenum)f, get_gl_format_type(f), data);
		if (gen_mips)
			glGenerateMipmap(GL_TEXTURE_2D);
	}		
	
	const GLchar* generate_tex_name(int slot)
	{
		GLchar* nm = new GLchar[32];
		sprintf(nm, "tex_%i", slot);
		return nm;
	}

	void texture2d::bind(device* dev, int slot, shader& s)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, _id);

		auto i = glGetUniformLocation(s.program_id(), generate_tex_name(slot));
		glUniform1i(i, slot);
	}

	void texture2d::unbind(device* dev, int slot)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	texture2d::~texture2d()
	{
		if (_id != 0)
			glDeleteTextures(1, &_id);
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