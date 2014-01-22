#pragma once
#include "cmmn.h"
#include "device.h"

namespace qeg
{
	struct vertex_position
	{
#ifdef DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC d3d_input_layout[];
#endif
		vec3 pos;
		vertex_position(vec3 p)
			: pos(p)
		{
		}
	};

	struct vertex_position_normal_texture
	{
#ifdef DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC d3d_input_layout[];
#endif
		vec3 pos;
		vec3 norm;
		vec2 tex;
		vertex_position_normal_texture(vec3 p, vec3 n, vec2 t)
			: pos(p), norm(n), tex(t)
		{
		}
	};

	enum class prim_draw_type
	{
#ifdef DIRECTX
		triangle_list = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		triangle_strip = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		line_list = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		line_strip = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		point_list = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
#endif
#ifdef OPENGL
		triangle_list = GL_TRIANGLES,
		triangle_strip = GL_TRIANGLE_STRIP,
		list_list = GL_LINES,
		line_strip = GL_LINE_STRIP,
		point_list = GL_POINTS,
#endif
	};
	
	template <typename vertex_type, typename index_type = uint32>
	class mesh
	{
		string _name;
		vector<vertex_type> _vertices;
		vector<index_type> _indices;
#ifdef DIRECTX
		ComPtr<ID3D11Buffer> vertex_buf;
		ComPtr<ID3D11Buffer> index_buf;
		ComPtr<ID3D11InputLayout> inplayo;
#endif
#ifdef OPENGL
		GLuint vtx_array;
		GLuint bufid[2];
#endif
	public:
		mesh(){}
		mesh(device* _dev, const string& n, const vector<vertex_type> v, const vector<index_type> i);
		~mesh();

		void draw(device* _dev, prim_draw_type drawtype = prim_draw_type::triangle_list);

		proprw(string, name, { return _name; });
		propr(vector<vertex_type>, vertices, { return _vertices; });
		propr(vector<index_type>, indices, { return _indices; });

#ifdef DIRECTX
		propr(ComPtr<ID3D11Buffer>, vertex_buffer, { return vertex_buf; });
		propr(ComPtr<ID3D11Buffer>, index_buffer, { return index_buf; });
		propr(ComPtr<ID3D11InputLayout>, input_layout, { return inplayo; });
#endif
#ifdef OPENGL
		propr(GLuint, vertex_array, { return vtx_array; });
		propr(GLuint, vertex_buffer, { return bufid[0]; });
		propr(GLuint, index_buffer, { return bufid[1]; });
#endif
	};
}

