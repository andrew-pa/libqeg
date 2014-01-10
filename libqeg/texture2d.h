#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	class texture2d
	{
	protected:
#ifdef DIRECTX
		ComPtr<ID3D11Texture2D> texd;
		ComPtr<ID3D11ShaderResourceView> srv;
#endif
#ifdef OPENGL
		GLuint _id;
#endif
		vec2 _size;
	public:
		texture2d(){}
		~texture2d();

		void bind(device& dev, int slot);
		void unbind(device& dev, int slot);

		propr(vec2, size, { return _size; });
#ifdef DIRECTX
		propr(ComPtr<ID3D11Texture2D>, texture2D, { return texd; });
		propr(ComPtr<ID3D11ShaderResourceView>, shader_resource_view, { return srv; });

		texture2d(device& dev, CD3D11_TEXTURE2D_DESC desc, CD3D11_SHADER_RESOURCE_VIEW_DESC srvdesc);
#endif
#ifdef OPENGL
		propr(GLuint, id, { return _id; });
#endif
	};
}

