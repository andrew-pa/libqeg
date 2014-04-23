#include "texture.h"

#ifdef DIRECTX
#include "dds_loader.h"
#elif OPENGL
#include <SOIL.h>
#endif

namespace qeg
{
#ifdef DIRECTX
	texture1d::texture1d(device* dev, uint size_, pixel_format f, void* data)
	{
		CD3D11_TEXTURE1D_DESC dsc((DXGI_FORMAT)f, size_, 1U, 0U, D3D11_BIND_SHADER_RESOURCE);
		if (data != nullptr)
		{
			dsc.MipLevels = 7;
			dsc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			dsc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		CD3D11_SHADER_RESOURCE_VIEW_DESC sd(D3D11_SRV_DIMENSION_TEXTURE1D, (DXGI_FORMAT)f);
		D3D11_SUBRESOURCE_DATA d = { 0 };
		d.pSysMem = data;
		chr(dev->ddevice()->CreateTexture1D(&dsc, (data == nullptr ? nullptr : &d), &texd));
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &sd, &srv));
	}
	texture1d::texture1d(device* dev, uint size_, pixel_format f, vector<void*> mip_data)
	{
		if (mip_data.size() <= 1) throw exception("need to have more than 1 mip level for loaded mipmaps");
		CD3D11_TEXTURE1D_DESC dsc((DXGI_FORMAT)f, size_, 1U, mip_data.size(), D3D11_BIND_SHADER_RESOURCE);
		CD3D11_SHADER_RESOURCE_VIEW_DESC sd(D3D11_SRV_DIMENSION_TEXTURE1D, (DXGI_FORMAT)f);
		vector<D3D11_SUBRESOURCE_DATA> srda;
		for(const auto& m : mip_data)
		{
			D3D11_SUBRESOURCE_DATA srd = { 0 };
			srd.pSysMem = m;
			srda.push_back(srd);
		}
		chr(dev->ddevice()->CreateTexture1D(&dsc, srda.data(), &texd));
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

	texture2d::texture2d(device* dev, uvec2 size_, pixel_format f, void* data)
		: texture(size_)
	{
		CD3D11_TEXTURE2D_DESC txd((DXGI_FORMAT)f, size_.x, size_.y, 1U, 0U,
			D3D11_BIND_SHADER_RESOURCE);
		CD3D11_SHADER_RESOURCE_VIEW_DESC srd(D3D11_SRV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)f);
		if (data != nullptr)
		{
			txd.MipLevels = 7;
			txd.BindFlags |= D3D11_BIND_RENDER_TARGET;
			txd.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		D3D11_SUBRESOURCE_DATA initdata = { 0 };
		initdata.pSysMem = data;
		initdata.SysMemPitch = size_.y * bytes_per_pixel(f);
		D3D11_SUBRESOURCE_DATA initdatas[7] = { initdata, initdata, initdata, initdata, initdata, initdata, initdata };
		chr(dev->ddevice()->CreateTexture2D(&txd, (data == nullptr ? nullptr : initdatas), &texd));
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srd, &srv));
	}
	texture2d::texture2d(device* dev, uvec2 size_, pixel_format f, vector<void*> mip_data)
		: texture(size_)
	{
		if (mip_data.size() <= 1) throw exception("not enougth mip levels");
		CD3D11_TEXTURE2D_DESC txd((DXGI_FORMAT)f, size_.x, size_.y, 1U, mip_data.size()-1,
			D3D11_BIND_SHADER_RESOURCE);
		CD3D11_SHADER_RESOURCE_VIEW_DESC srd(D3D11_SRV_DIMENSION_TEXTURE2D, (DXGI_FORMAT)f, 0U);
		vector<D3D11_SUBRESOURCE_DATA> srda;
		for (uint i = 0; i < mip_data.size(); ++i)
		{
			auto m = mip_data[i];
			D3D11_SUBRESOURCE_DATA srd = { 0 };
			srd.pSysMem = m;
			srd.SysMemPitch = (UINT)((size_.y / pow(2.f, i)) * bytes_per_pixel(f));
			srda.push_back(srd);
		}
		chr(dev->ddevice()->CreateTexture2D(&txd, srda.data(), &texd));
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

	texture3d::texture3d(device* dev, uvec3 size_, pixel_format f, void* data, bool gen_mips, size_t sys_slice_pitch, size_t sys_pitch)
		: texture(size_)
	{
		CD3D11_TEXTURE3D_DESC txd((DXGI_FORMAT)f, size_.x, size_.y, 1U, 0U,
			D3D11_BIND_SHADER_RESOURCE);		
		if (gen_mips)
		{
			txd.MipLevels = 7;
			txd.BindFlags |= D3D11_BIND_RENDER_TARGET;
			txd.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		CD3D11_SHADER_RESOURCE_VIEW_DESC srd(D3D11_SRV_DIMENSION_TEXTURE3D, (DXGI_FORMAT)f);
		D3D11_SUBRESOURCE_DATA initdata = { 0 };
		initdata.pSysMem = data;
		initdata.SysMemPitch = sys_pitch;
		initdata.SysMemSlicePitch = sys_slice_pitch;
		chr(dev->ddevice()->CreateTexture3D(&txd, (data == nullptr ? nullptr : &initdata), &texd));
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srd, &srv));
	}

	texture3d::texture3d(device* dev, CD3D11_TEXTURE3D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc)
		: texture(uvec3(desc.Width, desc.Height, desc.Depth))
	{
		chr(dev->ddevice()->CreateTexture3D(&desc, nullptr, &texd));
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srvdesc, &srv));
	}
	texture3d::texture3d(device* dev, CD3D11_TEXTURE3D_DESC desc)
		: texture(uvec3(desc.Width, desc.Height, desc.Depth))
	{
		chr(dev->ddevice()->CreateTexture3D(&desc, nullptr, &texd));
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc(texd.Get(), desc.Format);
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srvdesc, &srv));
	}

	texture3d::texture3d(ComPtr<ID3D11ShaderResourceView> srv_)
		: texture(srv_)
	{
		ComPtr<ID3D11Resource> rs;
		srv->GetResource(&rs);
		rs.As(&texd);
	}

	inline CD3D11_TEXTURE2D_DESC __modtd(CD3D11_TEXTURE2D_DESC& d)
	{
		d.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
		return d;
	}
	inline CD3D11_SHADER_RESOURCE_VIEW_DESC __modsrvd(CD3D11_SHADER_RESOURCE_VIEW_DESC& d)
	{
		d.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		return d;
	}

	//data_per_face order +x, -x, +y, -y, +z, -z : like dx?

	textureCube::textureCube(device* dev, uvec2 size_, pixel_format f, vector<byte*> data_per_face, bool gen_mips, size_t sys_pitch)
		: texture2d(size_)
	{
		if (data_per_face.size() < 6)
			throw exception("not enough data for all faces of cubemap");
		CD3D11_TEXTURE2D_DESC txd((DXGI_FORMAT)f, size_.x, size_.y, 1U, 0U,
			D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0U, 1U, 0U, D3D11_RESOURCE_MISC_TEXTURECUBE);
		CD3D11_SHADER_RESOURCE_VIEW_DESC srd(D3D11_SRV_DIMENSION_TEXTURECUBE, (DXGI_FORMAT)f); //make gen mips functional
		if (gen_mips)
		{
			txd.MipLevels = 7;
			txd.BindFlags |= D3D11_BIND_RENDER_TARGET;
			txd.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		size_t facesize_bytes = (size_.x * size_.y * bytes_per_pixel(f));
		byte* data = new byte[facesize_bytes * 6];
		for (int i = 0; i < 6; ++i)
			memcpy(data + (i*facesize_bytes), data_per_face[i], facesize_bytes);

		D3D11_SUBRESOURCE_DATA initdata = { 0 };
		initdata.pSysMem = data;
		initdata.SysMemPitch = sys_pitch;
		chr(dev->ddevice()->CreateTexture2D(&txd, &initdata, &texd));
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srd, &srv));

		delete[] data;
	}



	textureCube::textureCube(device* dev, CD3D11_TEXTURE2D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc)
		: texture2d(dev, __modtd(desc), __modsrvd(srvdesc)) //texture(uvec2(desc.Width, desc.Height))
	{
		chr(dev->ddevice()->CreateTexture2D(&desc, nullptr, &texd));
		chr(dev->ddevice()->CreateShaderResourceView(texd.Get(), &srvdesc, &srv));
	}
	textureCube::textureCube(device* dev, CD3D11_TEXTURE2D_DESC desc)
		: texture2d(dev, __modtd(desc), CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION_TEXTURECUBE, desc.Format))
	{
	}

	textureCube::textureCube(ComPtr<ID3D11ShaderResourceView> srv_)
		: texture2d(srv_)
	{
	}
#endif

#ifdef OPENGL
	texture1d::texture1d(device* dev, uint size_, pixel_format f, void* data)
		: texture(size_)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_1D, _id);
		glTexImage1D(GL_TEXTURE_1D, 0, detail::get_gl_format_internal(f),
			size_, 0, (GLenum)f, detail::get_gl_format_type(f), data);
		if (data != nullptr)
			glGenerateMipmap(GL_TEXTURE_1D);
	}
	
	texture1d::texture1d(device* dev, uint size_, pixel_format f, vector<void*> mip_data)
		: texture(size_)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_1D, _id);
		glTexStorage1D(GL_TEXTURE_1D, mip_data.size(), (GLenum)f, size_);
		for (uint i = 0; i < mip_data.size(); ++i)
		{
			uint mipmap_scale = (uint)floor(1.f / pow(2.f, (float)i));
			glTexSubImage1D(GL_TEXTURE_1D, i, 0, size_*mipmap_scale, detail::get_gl_format_internal(f),
				detail::get_gl_format_type(f), mip_data[i]);
		}
	}

	texture2d::texture2d(device* dev, uvec2 size_, pixel_format f, void* data)
		: texture(size_)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)(f), 
			size_.x, size_.y, 0, detail::get_gl_format_internal(f), detail::get_gl_format_type(f), data);
		if (data != nullptr)
			glGenerateMipmap(GL_TEXTURE_2D);
	}		

	texture2d::texture2d(device* dev, uvec2 size_, pixel_format f, vector<void*> mip_data)
		: texture(size_)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		glTexStorage2D(GL_TEXTURE_2D, mip_data.size(), (GLenum)f, size_.x, size_.y);
		for (uint i = 0; i < mip_data.size(); ++i)
		{
			uint mipmap_scale = (uint)floor(pow(2.f, (float)i));
			glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0,
				size_.x/mipmap_scale, size_.y/mipmap_scale, 
				detail::get_gl_format_internal(f), detail::get_gl_format_type(f), mip_data[i]);
		}
	}

	texture3d::texture3d(device* dev, uvec3 size_, pixel_format f, void* data, bool gen_mips, size_t, size_t)
		: texture(size_)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_3D, _id);
		glTexImage3D(GL_TEXTURE_3D, 0, detail::get_gl_format_internal(f),
			size_.x, size_.y, size_.z, 0, (GLenum)f, detail::get_gl_format_type(f), data);
		if (gen_mips)
			glGenerateMipmap(GL_TEXTURE_3D);
	}

	textureCube::textureCube(device* dev, uvec2 size_, pixel_format f, vector<byte*> data_per_face, bool gen_mips, size_t)
		: texture2d(size_)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

		for (uint face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			face < GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; ++face)
		{
			uint idx = face - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			glTexImage2D(face, 0, detail::get_gl_format_internal(f),
				size_.x, size_.y, 0, (GLenum)f,
				detail::get_gl_format_type(f), data_per_face[idx]);
		}
		
		if (gen_mips) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
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