#pragma once
#include "cmmn.h"
#include "device.h"
#include "shader.h"

namespace qeg
{
#ifdef OPENGL
	namespace detail
	{
		template<int D>
		struct gl_texture_slot_id_from_dim
		{
			static const uint value = -1;
		};

		struct gl_texture_slot_id_from_dim<1>
		{
			static const uint value = GL_TEXTURE_1D;
		};
	}
#endif
	template <int Dim>
	class texture
	{
	public:
		typedef typename vec_of<Dim, uint, highp>::x dim_type;
	protected:
		dim_type _size;
#ifdef DIRECTX
		ComPtr<ID3D11ShaderResourceView> srv;
		texture(ComPtr<ID3D11ShaderResourceView> srv_)
			: srv(srv_){}
#elif OPENGL
		GLuint _id;
		texture(dim_type s_, GLuint _i)
			: _size(s_), _id(_i){}
#endif
	public:
		texture() {}
		texture(dim_type _s)
			: _size(_s)	{}

		virtual void bind(device* dev, int slot, shader_stage ss, shader& s)
#ifdef DIRECTX
		{
			switch (ss)
			{
			case qeg::shader_stage::pixel_shader:
				dev->context()->PSSetShaderResources(slot, 1, srv.GetAddressOf());
				return;
			case qeg::shader_stage::vertex_shader:
				dev->context()->VSSetShaderResources(slot, 1, srv.GetAddressOf());
				return;
			}
		}
#elif OPENGL
			= 0;
#endif
		virtual void unbind(device* dev, int slot, shader_stage ss)
#ifdef DIRECTX
		{
			ID3D11ShaderResourceView* srvnll[] = { nullptr };
			switch (ss)
			{
			case qeg::shader_stage::pixel_shader:
				dev->context()->PSSetShaderResources(slot, 1, srvnll);
				return;
			case qeg::shader_stage::vertex_shader:
				dev->context()->VSSetShaderResources(slot, 1, srvnll);
				return;
			}
		}
#elif OPENGL
			= 0;
#endif

		propr(dim_type, size, { return _size; });
#ifdef DIRECTX
		propr(ComPtr<ID3D11ShaderResourceView>, shader_resource_view, { return srv; });
#elif OPENGL
		propr(GLuint, id, { return _id; });
#endif

		virtual ~texture()
		{
#ifdef OPENGL
			glDeleteTextures(1, &_id);
#endif
		}
	};

	class texture1d : public texture<1>
	{
#ifdef DIRECTX
		ComPtr<ID3D11Texture1D> texd;
		texture1d(ComPtr<ID3D11ShaderResourceView> srv_);
#endif
	public:
		texture1d(){}
		texture1d(uint len)
			: texture(len){}

		texture1d(device* dev, uint len, buffer_format f, void* data = nullptr, bool gen_mips = false
#ifdef DIRECTX
			, size_t sys_pitch = 4
#endif
			);

#ifdef OPENGL
		void bind(device* dev, int slot, shader_stage ss, shader& s) override;
		void unbind(device* dev, int slot, shader_stage ss) override;
#endif

#ifdef DIRECTX
		propr(ComPtr<ID3D11Texture1D>, texture2D, { return texd; });

		texture1d(device* dev, CD3D11_TEXTURE1D_DESC desc);
		texture1d(device* dev, CD3D11_TEXTURE1D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc);
#endif

	};

	class texture2d : public texture<2>
	{
	protected:
#ifdef DIRECTX
		ComPtr<ID3D11Texture2D> texd;
		texture2d(ComPtr<ID3D11ShaderResourceView> srv_);
#endif
	public:
		texture2d(){}

		texture2d(uvec2 _s)
			: texture(_s) { }

		texture2d(device* dev, uvec2 size, buffer_format f, void* data = nullptr, bool gen_mips = false
#ifdef DIRECTX
			, size_t sys_pitch = 4
#endif
			);

#ifdef OPENGL
		void bind(device* dev, int slot, shader_stage ss, shader& s) override;
		void unbind(device* dev, int slot, shader_stage ss) override;
#endif

		static texture2d* load_dds(device* dev, datablob<byte>* data);
		//static texture2d* load_bmp(device& dev, datablob<byte>* data)

#ifdef DIRECTX
		propr(ComPtr<ID3D11Texture2D>, texture2D, { return texd; });
		texture2d(device* dev, CD3D11_TEXTURE2D_DESC desc);
		texture2d(device* dev, CD3D11_TEXTURE2D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc);
#endif

	};
};

