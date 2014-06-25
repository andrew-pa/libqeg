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

	void sampler_state::bind(device* _dev, int slot, shader_stage ss, texture_dimension d)
	{
#ifdef DIRECTX
		switch (ss)
		{
		case qeg::shader_stage::pixel_shader:
			_dev->context()->PSSetSamplers(slot, 1, sampl_state.GetAddressOf());
			return;
		case qeg::shader_stage::vertex_shader:
			_dev->context()->VSSetSamplers(slot, 1, sampl_state.GetAddressOf());
			return;
		}
#elif OPENGL
		glActiveTexture(GL_TEXTURE0 + slot);

		GLenum glxd = detail::get_gl_txdm_from_pi(d);

		if(min_filter == texture_filter::anisotropic)
		{
			glTexParameteri(glxd, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(glxd, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(glxd, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLfloat)max_anisotropy);
		}
		else
		{
			glTexParameteri(glxd, GL_TEXTURE_MAG_FILTER, 
				(min_filter == texture_filter::point ? GL_NEAREST : GL_LINEAR));
			if(mip_filter == texture_filter::point)
			{
				glTexParameteri(glxd, GL_TEXTURE_MIN_FILTER,
					(min_filter == texture_filter::point ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST));
			}
			else if(mip_filter == texture_filter::linear)
			{
				glTexParameteri(glxd, GL_TEXTURE_MIN_FILTER,
					(min_filter == texture_filter::point ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR));
			}
		}
		glTexParameterfv(glxd, GL_TEXTURE_BORDER_COLOR, &border_color[0]);
		glTexParameterf(glxd, GL_TEXTURE_LOD_BIAS, mip_lod_bias);
		glTexParameterf(glxd, GL_TEXTURE_MIN_LOD, min_lod);
		glTexParameterf(glxd, GL_TEXTURE_MAX_LOD, max_lod);
		glTexParameteri(glxd, GL_TEXTURE_WRAP_S, (GLint)address_modes[0]);
		glTexParameteri(glxd, GL_TEXTURE_WRAP_T, (GLint)address_modes[1]);
		glTexParameteri(glxd, GL_TEXTURE_WRAP_R, (GLint)address_modes[2]);
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

	void sampler_state::unbind(device* _dev, int slot, shader_stage ss)
	{
#ifdef DIRECTX
		ID3D11SamplerState* nullsmps[] = { nullptr };		
		switch (ss)
		{
		case qeg::shader_stage::pixel_shader:
			_dev->context()->PSSetSamplers(slot, 1, nullsmps);
			return;
		case qeg::shader_stage::vertex_shader:
			_dev->context()->VSSetSamplers(slot, 1, nullsmps);
			return;
		}
#elif OPENGL
		//could do something here, but why?
#endif
	}




	rasterizer_state::rasterizer_state(device* _dev, fill_mode fm, cull_mode cm,
		bool fcc, int db,  float ssdb)
		: fillmode(fm), cullmode(cm), front_tri_cw(fcc), depth_bias(db), slope_scaled_depth_bias(ssdb)
	{
#ifdef DIRECTX
		CD3D11_RASTERIZER_DESC rsd((D3D11_FILL_MODE)fm, (D3D11_CULL_MODE) cm, fcc, db, 0.f, ssdb, TRUE, FALSE, FALSE, FALSE);
		_dev->ddevice()->CreateRasterizerState(&rsd, &rss);
#elif OPENGL
#endif
	}

	void rasterizer_state::bind(device* _dev)
	{
#ifdef DIRECTX
		_dev->context()->RSSetState(rss.Get());
#elif OPENGL
		if (cullmode == cull_mode::none)
			glDisable(GL_CULL_FACE);
		else
		{
			glEnable(GL_CULL_FACE);
			glCullFace((GLenum)cullmode);
			
		}
		glFrontFace(front_tri_cw ? GL_CW : GL_CCW);
		glPolygonMode(GL_FRONT_AND_BACK, (GLenum)fillmode);
		if (depth_bias != 0 || slope_scaled_depth_bias != 0)
			glPolygonOffset(slope_scaled_depth_bias, (GLfloat)depth_bias);
		else
			glPolygonOffset(0, 0);
#endif
	}

	void rasterizer_state::update(device* _dev)
	{
#ifdef DIRECTX
		CD3D11_RASTERIZER_DESC rsd((D3D11_FILL_MODE)fillmode, (D3D11_CULL_MODE)cullmode, front_tri_cw, depth_bias, 0.f, slope_scaled_depth_bias, TRUE, FALSE, FALSE, FALSE);
		_dev->ddevice()->CreateRasterizerState(&rsd, &rss);
#elif OPENGL
#endif
	}

	void rasterizer_state::unbind(device* _dev)
	{
#ifdef DIRECTX
		_dev->_bind_default_rs_state();
#elif OPENGL
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPolygonOffset(0, 0);
#endif
	}

	blend_state::blend_state(device* _dev, initializer_list<render_target_blend_state_desc> render_targets_bld)
#ifdef DIRECTX
	{
		D3D11_BLEND_DESC bd = { 0 };
		bd.IndependentBlendEnable = false;
		vector<render_target_blend_state_desc> rtbld(render_targets_bld.begin(), render_targets_bld.end());
		if(rtbld.size() < 8)
		{
			for (int j = rtbld.size(); j < 8; ++j)
			{
				rtbld.push_back(render_target_blend_state_desc());
			}
		}
		int i = 0;
		for(const auto& d : rtbld)
		{
			auto& rd = bd.RenderTarget[i];
			rd.BlendEnable = d.enabled;
			rd.SrcBlend = (D3D11_BLEND)d.src_factor;
			rd.DestBlend = (D3D11_BLEND)d.dest_factor;
			rd.BlendOp = (D3D11_BLEND_OP)d.op;
			rd.SrcBlendAlpha = (D3D11_BLEND)d.src_alpha_factor;
			rd.DestBlendAlpha = (D3D11_BLEND)d.dest_alpha_factor;
			rd.BlendOpAlpha = (D3D11_BLEND_OP)d.alpha_op;
			rd.RenderTargetWriteMask = (uint8)d.writemask;
			i++;
		}
		chr(_dev->ddevice()->CreateBlendState(&bd, &bss));
	}
#elif OPENGL
	{
		vector<render_target_blend_state_desc> rtbld(render_targets_bld.begin(), render_targets_bld.end());
		if(rtbld.size() < 8)
		{
			for (int j = rtbld.size(); j < 8; ++j)
			{
				rtbld.push_back(render_target_blend_state_desc()); 
			}
		}
		for (int i = 0; i < 8; ++i)
			render_targets[i] = rtbld[i];
	}
#endif

	void blend_state::bind(device* _dev, vec4 blend_color, uint smask)
#ifdef DIRECTX
	{
		_dev->context()->OMSetBlendState(bss.Get(), (float*)&blend_color, smask);
	}
#elif OPENGL
	{
		for (int i = 0; i < 8; ++i)
		{
			auto rt = render_targets[i];
			if(rt.enabled)
			{
				glEnablei(GL_BLEND, i);
				glBlendEquationSeparate((GLenum)rt.op, (GLenum)rt.alpha_op);
				glBlendFuncSeparate((GLenum)rt.src_factor, (GLenum)rt.dest_factor,
					(GLenum)rt.src_alpha_factor, (GLenum)rt.dest_alpha_factor);
				glBlendColor(blend_color.r, blend_color.g, blend_color.b, blend_color.a);
			//	glColorMaski(i, check_flag(rt.writemask, write_mask::enable_red), 
			//		check_flag(rt.writemask, write_mask::enable_green),
			//		check_flag(rt.writemask, write_mask::enable_blue),
			//		check_flag(rt.writemask, write_mask::enable_alpha));
			}
			else
			{
				glDisablei(GL_BLEND, i);
			}
		}
	}
#endif
	
	void blend_state::update(device* _dev)
#ifdef DIRECTX
	{
		D3D11_BLEND_DESC bd = { 0 };
		bd.IndependentBlendEnable = false;
		for (int i = 0; i < 8; ++i)
		{
			auto d = render_targets[i];
			auto& rd = bd.RenderTarget[i];
			rd.BlendEnable = d.enabled;
			rd.SrcBlend = (D3D11_BLEND)d.src_factor;
			rd.DestBlend = (D3D11_BLEND)d.dest_factor;
			rd.BlendOp = (D3D11_BLEND_OP)d.op;
			rd.SrcBlendAlpha = (D3D11_BLEND)d.src_alpha_factor;
			rd.DestBlendAlpha = (D3D11_BLEND)d.dest_alpha_factor;
			rd.BlendOpAlpha = (D3D11_BLEND_OP)d.alpha_op;
			rd.RenderTargetWriteMask = (uint8)d.writemask;
			i++;
		}
		chr(_dev->ddevice()->CreateBlendState(&bd, &bss));
	}
#elif OPENGL
	{}
#endif

	void blend_state::unbind(device* _dev)
#ifdef DIRECTX
	{
		_dev->context()->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}
#elif OPENGL
	{
		for (int i = 0; i < 8; ++i)
		{
			glDisablei(GL_BLEND, i);
			glBlendColor(1, 1, 1, 1);
			glColorMaski(i, true, true, true, true);
		}
	}
#endif

	depth_stencil_state::depth_stencil_state(device* _dev, bool depthenable, bool depthwritemask,
		comparison_func depthfunc, bool stencilenable,
		uint8 stencilwritemask, uint8 stencilreadmask,
		depthstencil_op frontdsop, depthstencil_op backdsop)
		: depth_enable(depthenable), allow_depthstencil_writes(depthwritemask), depth_comp(depthfunc),
		stencil_enable(stencilenable), stencil_write_mask(stencilwritemask), stencil_read_mask(stencilreadmask),
		frontface_op(frontdsop), backface_op(backdsop)
#ifdef DIRECTX
	{
		CD3D11_DEPTH_STENCIL_DESC dsd(depth_enable, (allow_depthstencil_writes ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO),
			(D3D11_COMPARISON_FUNC)depth_comp, stencil_enable, (UINT8)stencil_read_mask, (UINT8)stencil_write_mask, 
			(D3D11_STENCIL_OP)frontface_op.fail_op, (D3D11_STENCIL_OP)frontface_op.depth_fail_op, 
			(D3D11_STENCIL_OP)frontface_op.pass_op, (D3D11_COMPARISON_FUNC)frontface_op.stencil_comp,
			(D3D11_STENCIL_OP)backface_op.fail_op, (D3D11_STENCIL_OP)backface_op.depth_fail_op, 
			(D3D11_STENCIL_OP)backface_op.pass_op, (D3D11_COMPARISON_FUNC)backface_op.stencil_comp);
		_dev->ddevice()->CreateDepthStencilState(&dsd, dss.GetAddressOf());
	}
#elif  OPENGL
	{}
#endif

#ifdef OPENGL
	GLenum convert_compfunc(comparison_func cf) 
	{
		switch (cf)
		{
		case qeg::comparison_func::never:
			return GL_NEVER;
		case qeg::comparison_func::less:
			return GL_LESS;
		case qeg::comparison_func::equal:
			return GL_EQUAL;
		case qeg::comparison_func::less_equal:
			return GL_LEQUAL;
		case qeg::comparison_func::greater:
			return GL_GREATER;
		case qeg::comparison_func::not_equal:
			return GL_NOTEQUAL;
		case qeg::comparison_func::greater_equal:
			return GL_GEQUAL;
		case qeg::comparison_func::always:
			return GL_ALWAYS;
		}
		throw exception("invalid comparison function");
	}
	GLenum convert_sop(stencil_op o) 
	{
		switch (o)
		{
		case qeg::stencil_op::keep:
			return GL_KEEP;
		case qeg::stencil_op::zero:
			return GL_ZERO;
		case qeg::stencil_op::replace:
			return GL_REPLACE;
		case qeg::stencil_op::incr_sat:
			return GL_INCR;
		case qeg::stencil_op::decr_sat:
			return GL_DECR;
		case qeg::stencil_op::invert:
			return GL_INVERT;
		case qeg::stencil_op::incr:
			return GL_INCR_WRAP;
		case qeg::stencil_op::decr:
			return GL_DECR_WRAP;
		}
		throw exception("invalid stencil op");
	}
#endif

	void depth_stencil_state::bind(device* _dev, uint srf)
#ifdef DIRECTX
	{
		_dev->context()->OMSetDepthStencilState(dss.Get(), srf);
	}
#elif  OPENGL
	{
		if (depth_enable && allow_depthstencil_writes)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(convert_compfunc(depth_comp));
		}
		else glDisable(GL_DEPTH_TEST);
		if (stencil_enable && allow_depthstencil_writes) 
		{
			glEnable(GL_STENCIL_TEST);

			glStencilOpSeparate(GL_FRONT, convert_sop(frontface_op.fail_op),
				convert_sop(frontface_op.depth_fail_op),
				convert_sop(frontface_op.pass_op));
			glStencilFuncSeparate(GL_FRONT,
				convert_compfunc(frontface_op.stencil_comp),
				srf, stencil_read_mask);


			glStencilOpSeparate(GL_BACK, convert_sop(backface_op.fail_op),
				convert_sop(backface_op.depth_fail_op),
				convert_sop(backface_op.pass_op));
			glStencilFuncSeparate(GL_BACK,
				convert_compfunc(backface_op.stencil_comp),
				srf, stencil_read_mask);

			glStencilMask(stencil_write_mask);
		}
		else glDisable(GL_STENCIL_TEST);
	}
#endif

	void depth_stencil_state::update(device* _dev)
#ifdef DIRECTX
	{
		dss.Reset();
		CD3D11_DEPTH_STENCIL_DESC dsd(depth_enable, (allow_depthstencil_writes ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO),
			(D3D11_COMPARISON_FUNC)depth_comp, stencil_enable, (UINT8)stencil_read_mask, (UINT8)stencil_write_mask,
			(D3D11_STENCIL_OP)frontface_op.fail_op, (D3D11_STENCIL_OP)frontface_op.depth_fail_op,
			(D3D11_STENCIL_OP)frontface_op.pass_op, (D3D11_COMPARISON_FUNC)frontface_op.stencil_comp,
			(D3D11_STENCIL_OP)backface_op.fail_op, (D3D11_STENCIL_OP)backface_op.depth_fail_op,
			(D3D11_STENCIL_OP)backface_op.pass_op, (D3D11_COMPARISON_FUNC)backface_op.stencil_comp);
		_dev->ddevice()->CreateDepthStencilState(&dsd, dss.GetAddressOf());
	}
#elif  OPENGL
	{
	}
#endif

	void depth_stencil_state::unbind(device* _dev)
#ifdef DIRECTX
	{
		_dev->context()->OMSetDepthStencilState(nullptr, 0);
	}
#elif OPENGL
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDisable(GL_STENCIL_TEST);
		glStencilMask(0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_ALWAYS, 0, 0xff);
	}
#endif
}











