#include "states.h"

namespace qeg
{
#ifdef DIRECTX
	static D3D11_FILTER d3d_translate_filter(texture_filter mf, texture_filter gf, texture_filter pf)
	{
		if (mf == texture_filter::anisotropic) return D3D11_FILTER_ANISOTROPIC;
		if(mf == texture_filter::point)
		{
			if(gf == texture_filter::point)
			{
				if (pf == texture_filter::point)
					return D3D11_FILTER_MIN_MAG_MIP_POINT;
				if (pf == texture_filter::linear)
					return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			}
			else if(gf == texture_filter::linear)
			{
				if (pf == texture_filter::point)
					return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
				if (pf == texture_filter::linear)
					return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
			}
		}
		else if(mf == texture_filter::linear)
		{
			if (gf == texture_filter::point)
			{
				if (pf == texture_filter::point)
					return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
				if (pf == texture_filter::linear)
					return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			}
			else if (gf == texture_filter::linear)
			{
				if (pf == texture_filter::point)
					return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				if (pf == texture_filter::linear)
					return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			}
		}
		throw exception("invalid texture filter");
	}
#endif

	sampler_state::sampler_state(device* _dev,
		texture_filter mf,
		texture_filter gf,
		texture_filter pf,
		texture_address_mode addressu,
		texture_address_mode addressv,
		texture_address_mode addressw,
		uint max_ansio, vec4 bordercolor,
		float mipLODbias, float minLOD, float maxLOD)
		: min_filter(mf), mag_filter(gf), mip_filter(pf),
			max_anisotropy(max_ansio), border_color(bordercolor),
			mip_lod_bias(mipLODbias), min_lod(minLOD), max_lod(maxLOD)
	{
		address_modes[0] = addressu;
		address_modes[1] = addressv;
		address_modes[2] = addressw;

#ifdef DIRECTX
		CD3D11_SAMPLER_DESC smp_desc(d3d_translate_filter(mf, gf, pf), (D3D11_TEXTURE_ADDRESS_MODE)addressu, (D3D11_TEXTURE_ADDRESS_MODE)addressv, (D3D11_TEXTURE_ADDRESS_MODE)addressw,
			mipLODbias, max_ansio, D3D11_COMPARISON_NEVER, &bordercolor[0], minLOD, maxLOD);
		_dev->ddevice()->CreateSamplerState(&smp_desc, &sampl_state);
#endif
	}

	void sampler_state::bind(device* _dev, int slot)
	{
#ifdef DIRECTX
		_dev->context()->PSSetSamplers(slot, 1, sampl_state.GetAddressOf());
#elif OPENGL
		glActiveTexture(GL_TEXTURE0 + slot);
		if(min_filter == texture_filter::anisotropic)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
				(min_filter == texture_filter::point ? GL_NEAREST : GL_LINEAR));
			if(mip_filter == texture_filter::point)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					(min_filter == texture_filter::point ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST));
			}
			else if(mip_filter == texture_filter::linear)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					(min_filter == texture_filter::point ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR));
			}
		}
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border_color[0]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, mip_lod_bias);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, min_lod);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, max_lod);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)address_modes[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)address_modes[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, (GLint)address_modes[2]);
#endif
	}

	void sampler_state::update(device* _dev)
	{
#ifdef DIRECTX
		sampl_state.Reset();
		CD3D11_SAMPLER_DESC smp_desc(d3d_translate_filter(min_filter, mag_filter, mip_filter), (D3D11_TEXTURE_ADDRESS_MODE)address_modes[0], (D3D11_TEXTURE_ADDRESS_MODE)address_modes[1], (D3D11_TEXTURE_ADDRESS_MODE)address_modes[2],
			mip_lod_bias, max_anisotropy, D3D11_COMPARISON_NEVER, &border_color[0], min_lod, max_lod);
		_dev->ddevice()->CreateSamplerState(&smp_desc, &sampl_state);
#endif
	}

	void sampler_state::unbind(device* _dev, int slot)
	{
#ifdef DIRECTX
		ID3D11SamplerState* nullsmps[] = { nullptr };
		_dev->context()->PSSetSamplers(slot, 1, nullsmps);
#elif OPENGL
		//could do something here, but why?
#endif
	}
}











