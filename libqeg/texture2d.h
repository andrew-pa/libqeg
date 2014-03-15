#pragma once
#include "cmmn.h"
#include "device.h"
#include "shader.h"

namespace qeg
{
	class texture2d
	{
	protected:
#ifdef DIRECTX
		ComPtr<ID3D11Texture2D> texd;
		ComPtr<ID3D11ShaderResourceView> srv;
		texture2d(ComPtr<ID3D11ShaderResourceView> srv_);
#endif
#ifdef OPENGL
		GLuint _id;
		texture2d(vec2 s_, GLuint i_)
			: _size(s_), _id(i_){}
#endif
		vec2 _size;
	public:
		texture2d(){}
		~texture2d();

		texture2d(vec2 _s)
			: _size(_s) { }

		texture2d(device* dev, vec2 size, buffer_format f, void* data = nullptr, bool gen_mips = false
#ifdef DIRECTX
			, size_t sys_pitch = 4
#endif
			);
		
		static texture2d* load_dds(device* dev, datablob<byte>* data);
		//static texture2d* load_bmp(device& dev, datablob<byte>* data);

		void bind(device* dev, int slot, shader_stage ss, shader& s);
		void unbind(device* dev, int slot, shader_stage ss);

		propr(vec2, size, { return _size; });
#ifdef DIRECTX
		propr(ComPtr<ID3D11Texture2D>, texture2D, { return texd; });
		propr(ComPtr<ID3D11ShaderResourceView>, shader_resource_view, { return srv; });

		texture2d(device& dev, CD3D11_TEXTURE2D_DESC desc);
		texture2d(device& dev, CD3D11_TEXTURE2D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc);
#endif
#ifdef OPENGL
		propr(GLuint, id, { return _id; });
#endif
	};
};

