#include "mesh.h"

namespace qeg
{
	template <typename index_type>
	struct format_for_index_type
	{
		static const pixel_format format = pixel_format::UNKNOWN;
	};

	template <>
	struct format_for_index_type<uint32>
	{
		static const pixel_format format = pixel_format::R32_UINT;
	};

	template <>
	struct format_for_index_type<uint16>
	{
		static const pixel_format format = pixel_format::R16_UINT;
	};

#ifdef DIRECTX
	const D3D11_INPUT_ELEMENT_DESC vertex_position::d3d_input_layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	const D3D11_INPUT_ELEMENT_DESC vertex_position_normal_texture::d3d_input_layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	template <typename vertex_type, typename index_type>
	mesh<vertex_type,index_type>::mesh(device* _dev, const string& n, const vector<vertex_type>& v, const vector<index_type>& i)
		: _name(n), vertices(v), indices(i)
	{
		D3D11_SUBRESOURCE_DATA vd = { 0 };
		vd.pSysMem = &v[0];
		CD3D11_BUFFER_DESC vbd(sizeof(vertex_type)*v.size(), D3D11_BIND_VERTEX_BUFFER);
		chr(_dev->ddevice()->CreateBuffer(&vbd, &vd, &vertex_buf);
		D3D11_SUBRESOURCE_DATA id = { 0 };
		id.pSysMem = &i[0];
		CD3D11_BUFFER_DESC ibd(sizeof(index_type)*i.size(), D3D11_BIND_INDEX_BUFFER);
		chr(_dev->ddevice()->CreateBuffer(&ibd, &id, &index_buf);
	}

	template <typename vertex_type, typename index_type>
	void mesh<vertex_type, index_type>::draw(device* _dev, prim_draw_type drawtype)
	{
		UINT stride = sizeof(vertex_type);
		UINT offset = 0;
		_dev->context()->IASetVertexBuffers(0, 1, vertex_buf.GetAddressOf(), &stride, &offset);
		_dev->context()->IASetIndexBuffer(index_buf.Get(), format_for_index_type<index_type>::format, 0);
		_dev->context()->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)drawtype);
		_dev->context()->DrawIndexed(indices.size(), 0, 0);
	
	}
#endif

#ifdef OPENGL
	template <typename vertex_type, typename index_type>
	mesh<vertex_type, index_type>::mesh(device* _dev, const string& n, const vector<vertex_type>& v, const vector<index_type>& i)
		: _name(n), vertices(v), indices(i)
	{
		glGenVertexArrays(1, &vtx_array);
		glBindVertexArray(vtx_array);
		glGenBuffers(2, bufid);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer());
		glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(vertex_type), &v[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, i.size()*sizeof(index_type), &i[0], GL_STATIC_DRAW);
	}

	template <typename vertex_type, typename index_type>
	void mesh<vertex_type, index_type>::draw(device* _dev, prim_draw_type drawtype)
	{
	}
#endif
}
