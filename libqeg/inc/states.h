#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	struct sampler_state
	{
	protected:
#ifdef DIRECTX
		ComPtr<ID3D11SamplerState> sampl_state;
#endif
	public:
		texture_filter min_filter;
		texture_filter mag_filter;
		texture_filter mip_filter;
		texture_address_mode address_modes[3]; //0:u, 1:v, 2:w || 0:s, 1:t, 2:r
		uint max_anisotropy;
		float mip_lod_bias;
		float min_lod;
		float max_lod;
		vec4 border_color;
		
		sampler_state(device* _dev, 
			texture_filter mf = texture_filter::anisotropic, 
			texture_filter gf = texture_filter::anisotropic, 
			texture_filter pf = texture_filter::anisotropic,
			texture_address_mode addressu = texture_address_mode::wrap, 
			texture_address_mode addressv = texture_address_mode::wrap,
			texture_address_mode addressw = texture_address_mode::wrap,
			uint max_ansio = 16, vec4 bordercolor = vec4(.5f, .5f, .5f, 1),
			float mipLODbias = 0, float minLOD = -FLT_MAX, float maxLOD = FLT_MAX);

		void bind(device* _dev, int slot, shader_stage ss, texture_dimension d = texture_dimension::texture_2d);
		void update(device* _dev);
		void unbind(device* _dev, int slot, shader_stage ss);
	};

	enum class fill_mode 
	{
#ifdef DIRECTX
		wireframe = D3D11_FILL_WIREFRAME,
		solid = D3D11_FILL_SOLID,
#elif OPENGL
		wireframe = GL_LINE,
		solid = GL_FILL,
#endif
	};

	enum class cull_mode
	{
#ifdef DIRECTX
		none = D3D11_CULL_NONE,
		front = D3D11_CULL_FRONT,
		back = D3D11_CULL_BACK,
#elif OPENGL
		none = -1,
		front = GL_FRONT,
		back = GL_BACK,
#endif
	};

	struct rasterizer_state
	{
#ifdef DIRECTX
		ComPtr<ID3D11RasterizerState> rss;
#endif
	public:
		fill_mode fillmode;
		cull_mode cullmode;
		bool front_tri_cw;
		int depth_bias;
		float slope_scaled_depth_bias;
		rasterizer_state(device* _dev, fill_mode fm = fill_mode::solid, cull_mode cm = cull_mode::back,
			bool fcc = 
#ifdef DIRECTX
			true, //stupid hack
#elif OPENGL
			false,
#endif
			int db = 0, float ssdb = 0);
		void bind(device* _dev);
		void update(device* _dev);
		void unbind(device* _dev);
	};

	enum class blend_factor
	{
#ifdef DIRECTX
		zero = 1,
		one,
		src_color,
		inv_src_color,
		src_alpha,
		inv_src_alpha,
		dest_alpha,
		inv_dest_alpha,
		dest_color,
		inv_dest_color,
		src_alpha_sat,
		blend_factor,
		inv_blend_factor,
		src1_color,
		inv_src1_color,
		src1_alpha,
		inv_src1_alpha,
#elif OPENGL
		zero = GL_ZERO,
		one = GL_ONE,
		src_color = GL_SRC_COLOR,
		inv_src_color = GL_ONE_MINUS_SRC_COLOR,
		src_alpha = GL_SRC_ALPHA,
		inv_src_alpha = GL_ONE_MINUS_SRC_ALPHA,
		dest_alpha = GL_DST_ALPHA,
		inv_dest_alpha = GL_ONE_MINUS_DST_ALPHA,
		dest_color = GL_DST_COLOR,
		inv_dest_color = GL_ONE_MINUS_DST_COLOR,
		src_alpha_sat = GL_SRC_ALPHA_SATURATE,
		blend_factor = GL_CONSTANT_ALPHA, //DX is weird
		inv_blend_factor = GL_ONE_MINUS_CONSTANT_ALPHA, //DX is weird
		src1_color = GL_SRC1_COLOR,
		inv_src1_color = GL_ONE_MINUS_SRC1_COLOR,
		src1_alpha = GL_SRC1_ALPHA,
		inv_src1_alpha = GL_ONE_MINUS_SRC1_ALPHA,
#endif
	};

	enum class blend_op
	{
#ifdef DIRECTX
		add = 1,
		subtract,
		rev_subtract,
		min, max
#elif OPENGL
		add = GL_FUNC_ADD,
		subtract = GL_FUNC_SUBTRACT,
		rev_subtract = GL_FUNC_REVERSE_SUBTRACT,
		min = GL_MIN,
		max = GL_MAX,
#endif
	};

	enum class write_mask : uint8
	{
		//works w/ DX and GL requires translation
		enable_red = 1,
		enable_green = 2,
		enable_blue = 4,
		enable_alpha = 8,
		enable_all = 15,//enable_red | enable_green | enable_blue | enable_alpha,
	};

	struct blend_state
	{
#ifdef DIRECTX
		ComPtr<ID3D11BlendState> bss;
#endif
	public:
		struct render_target_blend_state_desc
		{
			bool enabled;
			blend_factor src_factor;
			blend_factor dest_factor;
			blend_op op;

			blend_factor src_alpha_factor;
			blend_factor dest_alpha_factor;
			blend_op alpha_op;

			write_mask writemask;
			render_target_blend_state_desc(
				bool ena = false, 
				blend_factor srcf  = blend_factor::one, 
				blend_factor detf  = blend_factor::zero,
				blend_op	 cop	   = blend_op::add,
				blend_factor srcaf = blend_factor::one,
				blend_factor dstaf = blend_factor::zero,
				blend_op	 aop    = blend_op::add,
				write_mask wmask = write_mask::enable_all)
				: enabled(ena), src_factor(srcf), dest_factor(detf),
				op(cop), src_alpha_factor(srcaf), dest_alpha_factor(dstaf),
				alpha_op(aop), writemask(wmask){}
		};
		render_target_blend_state_desc	render_targets[8];

		blend_state(device* _dev, initializer_list<render_target_blend_state_desc> render_targets_bld);

		void bind(device* _dev, vec4 blend_color = vec4(1.f), uint sample_mask = 0xffffffff);
		void update(device* _dev);
		void unbind(device* _dev);
	};

	enum class comparison_func 
	{ 
		never = 1, less, equal, less_equal, greater, not_equal, greater_equal, always
	};

	enum class stencil_op 
	{
		keep = 1,
		zero, replace, incr_sat, decr_sat, invert, incr, decr,
	};

	struct depth_stencil_state 
	{
#ifdef DIRECTX
		ComPtr<ID3D11DepthStencilState> dss;
#endif
	public:
		bool allow_depthstencil_writes;
		bool depth_enable;
		comparison_func depth_comp;
		
		bool stencil_enable;
		uint8 stencil_read_mask;
		uint8 stencil_write_mask;

		struct depthstencil_op
		{
			stencil_op fail_op;
			stencil_op depth_fail_op;
			stencil_op pass_op;
			comparison_func stencil_comp;

			depthstencil_op(stencil_op f = stencil_op::keep, 
				stencil_op df = stencil_op::keep, stencil_op p = stencil_op::keep, 
				comparison_func cf = comparison_func::always) 
				: fail_op(f), depth_fail_op(df), pass_op(p), stencil_comp(cf) {}
		};

		depthstencil_op frontface_op;
		depthstencil_op backface_op;

		depth_stencil_state(device* _dev, bool depthenable = true, bool depthwritemask = true,
			comparison_func depthfunc = comparison_func::less, bool stencilenable = false,
			uint8 stencilwritemask = 0xff, uint8 stencilreadmask = 0xff,
			depthstencil_op frontdsop = depthstencil_op(), depthstencil_op backdsop = depthstencil_op());

		void bind(device* _dev, uint stencil_ref);
		void update(device* _dev);
		void unbind(device* _dev);
	};
}