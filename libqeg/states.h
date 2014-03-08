#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	enum class texture_filter
	{
		point, linear, anisotropic
	};

	enum class texture_address_mode
	{
#ifdef DIRECTX
		wrap = 1, //lines up with D3D11_TEXTURE_ADDRESS_MODE
		mirror,
		clamp,
		border,
		mirror_once,
#elif OPENGL
		wrap = GL_REPEAT,
		mirror = GL_MIRRORED_REPEAT,
		clamp = GL_CLAMP_TO_EDGE,
		border = GL_CLAMP_TO_BORDER,
		mirror_once = GL_MIRROR_CLAMP_TO_EDGE,
#endif
	};

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

		void bind(device* _dev, int slot, shader_stage ss);
		void update(device* _dev);
		void unbind(device* _dev, int slot, shader_stage ss);
	};

	enum class fill_mode 
	{
#ifdef DIRECTX
		wireframe = D3D11_FILL_WIREFRAME,
		solid = D3D11_FILL_SOLID,
#elif OPENGL
		//wireframe = GL_LINE,
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
}