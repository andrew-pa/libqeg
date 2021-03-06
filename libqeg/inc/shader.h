#pragma once
#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	class shader
	{
	protected:
#ifdef DIRECTX
		ComPtr<ID3D11VertexShader> vertex_sh;
		ComPtr<ID3D11PixelShader> pixel_sh;
		ComPtr<ID3D11GeometryShader> geom_sh;
		ComPtr<ID3D11InputLayout> inplo; //this should be in mesh, but it has a dependency on the VS bytecode only in DX, which is lame
#endif
#ifdef OPENGL
		GLuint _id, _idvp, _idfp, _idgp;
#endif
	public:
#ifdef DIRECTX
		shader(device* _dev, const datablob<byte>& vs_data, const datablob<byte>& ps_data, const D3D11_INPUT_ELEMENT_DESC lo[], size_t cnt);
#endif
		shader(device* _dev, const datablob<byte>& vs_data, const datablob<byte>& ps_data = datablob<byte>(),
			const datablob<byte>& gs_data = datablob<byte>());
		~shader();

		virtual void bind(device* _dev);
		virtual void unbind(device* _dev);
		virtual void update(device* _dev);

#ifdef DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC layout_posnomtex[];
		propr(ComPtr<ID3D11VertexShader>, vertex_shader, { return vertex_sh; });
		propr(ComPtr<ID3D11PixelShader>, pixel_shader, { return pixel_sh; });
#endif
#ifdef OPENGL
		propr(GLuint, program_id, { return _id; });
		propr(GLuint, vertex_shader_id, { return _idvp; });
		propr(GLuint, pixel_shader_id, { return _idfp; });
#endif
	};
}

