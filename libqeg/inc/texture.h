#pragma once
#include "cmmn.h"
#include "device.h"
#include "shader.h"
#include "resource.h"

namespace qeg
{
#ifdef OPENGL
	namespace detail
	{
		template<int D>
		struct gl_texture_slot_id_from_dim
		{
			static const GLenum value = -1;
		};

		template<>
		struct gl_texture_slot_id_from_dim<1>
		{
			static const GLenum value = GL_TEXTURE_1D;
		};
		template<>
		struct gl_texture_slot_id_from_dim<2>
		{
			static const GLenum value = GL_TEXTURE_2D;
		};
		template<>
		struct gl_texture_slot_id_from_dim<3>
		{
			static const GLenum value = GL_TEXTURE_3D;
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

		static const GLchar* generate_tex_name(int slot, shader_stage ss)
		{
			GLchar* nm = new GLchar[32];
			GLchar* shss = new GLchar[8];
			if (ss == shader_stage::vertex_shader)
				shss = "vs";
			else if (ss == shader_stage::pixel_shader)
				shss = "ps";
			sprintf(nm, "%s_tex_%i", shss, slot);
			return nm;
		}
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
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(detail::gl_texture_slot_id_from_dim<Dim>::value, _id);

			auto i = glGetUniformLocation(s.program_id(), generate_tex_name(slot, ss));
			glUniform1i(i, slot);
		}
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
		{
			glBindTexture(detail::gl_texture_slot_id_from_dim<Dim>::value, 0);
		}

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
			if(_id != 0)
			{
				glDeleteTextures(1, &_id);
				_id = 0;
			}
#endif
		}
	};

	class texture1d : public texture<1>
	{
#ifdef DIRECTX
		ComPtr<ID3D11Texture1D> texd;
		texture1d(ComPtr<ID3D11ShaderResourceView> srv_);
#elif OPENGL
		texture1d(dim_type w, GLuint i)
			: texture(w,i){}
#endif
	public:
		texture1d(){}
		texture1d(uint len)
			: texture(len){}

		texture1d(device* dev, uint len, pixel_format f, void* data = nullptr);
		texture1d(device* dev, uint len, pixel_format f, vector<void*> mip_data);

		inline static texture1d* load(device* _dev, const datablob<byte>& file)
		{
			return static_cast<texture1d*>(detail::_load_texture(_dev, file));
		}

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
#elif OPENGL
		texture2d(dim_type w, GLuint i)
			: texture(w, i){}
#endif
	public:
		texture2d(){}

		texture2d(uvec2 _s)
			: texture(_s) { }

		texture2d(device* dev, uvec2 size, pixel_format f, void* data = nullptr);
		texture2d(device* dev, uvec2 size, pixel_format f, vector<void*> mip_data);

		static texture2d* load_dds(device* dev, datablob<byte>* data);

		inline static texture2d* load(device* _dev, const datablob<byte>& file)
		{
			return static_cast<texture2d*>(detail::_load_texture(_dev, file));
		}

		//static texture2d* load_bmp(device& dev, datablob<byte>* data)

#ifdef DIRECTX
		propr(ComPtr<ID3D11Texture2D>, texture2D, { return texd; });
		texture2d(device* dev, CD3D11_TEXTURE2D_DESC desc);
		texture2d(device* dev, CD3D11_TEXTURE2D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc);
#endif

	};

	class texture3d : public texture<3>
	{
	protected:
#ifdef DIRECTX
		ComPtr<ID3D11Texture3D> texd;
		texture3d(ComPtr<ID3D11ShaderResourceView> srv_);
#elif OPENGL
		texture3d(dim_type w, GLuint i)
			: texture(w, i){}
#endif
	public:
		texture3d(){}

		texture3d(uvec3 _s)
			: texture(_s) { }

		texture3d(device* dev, uvec3 size, pixel_format f, void* data = nullptr, 
			bool gen_mips = false, size_t sys_slice_pitch = -1, size_t sys_pitch = 4);

		inline static texture2d* load(device* _dev, const datablob<byte>& file)
		{
			return static_cast<texture2d*>(detail::_load_texture(_dev, file));
		}

		//TODO: Implement or consider loading 3D mipmaps

#ifdef DIRECTX
		propr(ComPtr<ID3D11Texture3D>, texture3D, { return texd; });
		texture3d(device* dev, CD3D11_TEXTURE3D_DESC desc);
		texture3d(device* dev, CD3D11_TEXTURE3D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc);
#endif

	};


	class textureCube : public texture2d
	{
	protected:
#ifdef DIRECTX
		textureCube(ComPtr<ID3D11ShaderResourceView> srv_);
#endif
	public:
		textureCube(){}

		textureCube(device* dev, uvec2 size, pixel_format f, vector<byte*> data_per_face, 
			bool gen_mips = false,  size_t sys_pitch = 4);

		inline static textureCube* load(device* _dev, const datablob<byte>& file)
		{
			return static_cast<textureCube*>(detail::_load_texture(_dev, file));
		}

		//TODO: Implement or consider loading TexCube mipmaps
#ifdef DIRECTX
		textureCube(device* dev, CD3D11_TEXTURE2D_DESC desc);
		textureCube(device* dev, CD3D11_TEXTURE2D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc);
#endif

		static textureCube* load_dds(device* dev, datablob<byte>* data);
	};
};

