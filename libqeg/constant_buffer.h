#pragma once
#include "cmmn.h"
#include "device.h"
#include "shader.h"

namespace qeg
{
	template <typename T>
	class constant_buffer
	{
		T _data;
		bool changed;		
		int _slot;

#ifdef DIRECTX
		ComPtr<ID3D11Buffer> _buf;
		shader_stage _ss;
#endif
#ifdef OPENGL
		GLuint _buf;
		GLuint _ix;

		GLchar* generate_block_name(int slot, shader_stage shs)
		{
			ostringstream oss;
			if(shs == shader_stage::vertex_shader)
				oss << "vs_";
			else if(shs == shader_stage::pixel_shader)
				oss << "ps_";
			oss << "reg_" << slot;
			return (GLchar*)oss.str().c_str();
		}
#endif
	public:
		constant_buffer(device* _dev, const shader& sh, int slot,  T d = T(), shader_stage shs = shader_stage::pixel_shader)
			: _data(d), _slot(slot), changed(true)
#ifdef DIRECTX
			, _ss(shs)
		{
			CD3D11_BUFFER_DESC bd(sizeof(T), D3D11_BIND_CONSTANT_BUFFER);
			chr(_dev->ddevice()->CreateBuffer(&bd, nullptr, &_buf));
		}
#elif OPENGL
		{
			glGenBuffers(1, &_buf);
			glBindBuffer(GL_UNIFORM_BUFFER, _buf);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &_data, GL_DYNAMIC_DRAW);
			_ix = glGetUniformBlockIndex(sh.program_id(), generate_block_name(slot, shs));
			glUniformBlockBinding(sh.program_id(), _ix, slot);
		}
#endif
		~constant_buffer()
#ifdef DIRECTX 
		{}
#elif OPENGL
		{
			glDeleteBuffers(1, &_buf);
		}
#endif
		
		inline const T& data() { return _data; }
		inline void data(T& d) { changed = true; _data = d; }

		void bind(device* _dev,  int ovslot = -1)
#ifdef DIRECTX 
		{
			if (_ss == shader_stage::vertex_shader)
			{
				_dev->context()->VSSetConstantBuffers((ovslot > 0 ? ovslot : _slot), 1, _buf.GetAddressOf());
			}
			else if (_ss == shader_stage::pixel_shader)
			{
				_dev->context()->PSSetConstantBuffers((ovslot > 0 ? ovslot : _slot), 1, _buf.GetAddressOf());
			}
		}
#elif OPENGL
		{
			glBindBuffer(GL_UNIFORM_BUFFER, _buf);
			glBindBufferBase(GL_UNIFORM_BUFFER, _slot, _buf);
		}
#endif

		void unbind(device* _dev, int ovslot = -1)
#ifdef DIRECTX 
		{				
			ID3D11Buffer* nb[] = { nullptr };

			if (_ss == shader_stage::vertex_shader)
			{
				_dev->context()->VSSetConstantBuffers((ovslot > 0 ? ovslot : _slot), 1,nb);
			}
			else if (_ss == shader_stage::pixel_shader)
			{
				_dev->context()->PSSetConstantBuffers((ovslot > 0 ? ovslot : _slot), 1, nb);
			}
		}
#elif OPENGL
		{			
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glBindBufferBase(GL_UNIFORM_BUFFER, (ovslot > 0 ? ovslot : _slot), 0);
		}
#endif

		void update(device* _dev)
#ifdef DIRECTX 
		{
			if(!changed) return;
			changed = false;
			_dev->context()->UpdateSubresource(_buf.Get(), 0, nullptr, &_data, 0, 0);
		}
#elif OPENGL
		{
			if (!changed) return;
			changed = false;
			glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &_data, GL_DYNAMIC_DRAW);
		}
#endif


#ifdef DIRECTX
		proprw(int, slot, { return _slot; });
		propr(ComPtr<ID3D11Buffer>, buffer, { return _buf; });
#endif
	};
}

