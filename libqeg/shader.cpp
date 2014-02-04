#include "shader.h"

namespace qeg
{
#ifdef DIRECTX
	
	const D3D11_INPUT_ELEMENT_DESC shader::layout_posnomtex[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	shader::shader(device* _dev, datablob<byte>* vs_data, datablob<byte>* ps_data, 
		const D3D11_INPUT_ELEMENT_DESC lo[], size_t cnt)
	{
		if (vs_data != nullptr)
			chr(_dev->ddevice()->CreateVertexShader(vs_data->data, vs_data->length, nullptr, &vertex_sh));
		if (ps_data != nullptr)
			chr(_dev->ddevice()->CreatePixelShader(ps_data->data, ps_data->length, nullptr, &pixel_sh));
		chr(_dev->ddevice()->CreateInputLayout(lo, cnt, vs_data->data, vs_data->length, &inplo));
	}

	void shader::bind(device* _dev)
	{
		_dev->context()->VSSetShader(vertex_sh.Get(), nullptr, 0);
		_dev->context()->PSSetShader(pixel_sh.Get(), nullptr, 0);
		_dev->context()->IASetInputLayout(inplo.Get());
	}

	void shader::unbind(device* _dev)
	{
		_dev->context()->VSSetShader(nullptr, nullptr, 0);
		_dev->context()->PSSetShader(nullptr, nullptr, 0);
		_dev->context()->IASetInputLayout(nullptr);
	}

	void shader::update(device* _dev)
	{
	}

	shader::~shader()
	{
	}
#endif
#ifdef OPENGL
	static void validate_shader(GLuint shader, const string& fn)
	{
		char buf[512];
		GLsizei len = 0;
		OutputDebugStringA(fn.c_str());
		glGetShaderInfoLog(shader, 512, &len, buf);
		if (len > 0)
		{
			ostringstream oss;
			oss << "Shader " << shader << " (" << fn << ")" << " error: " << buf;
			OutputDebugStringA(buf);
			throw exception(oss.str().c_str());
		}
	}
	shader::shader(device* _dev, datablob<byte>* vs_data, datablob<byte>* ps_data)
	{
		if(vs_data != nullptr)
		{
			_idvp = glCreateShader(GL_VERTEX_SHADER);
			const GLchar* vsd = (GLchar*)vs_data->data;
			glShaderSource(_idvp, 1, &vsd, nullptr);
			glCompileShader(_idvp);
			validate_shader(_idvp, (GLchar*)vs_data->data);
		}

		if(ps_data != nullptr)
		{			
			_idfp = glCreateShader(GL_FRAGMENT_SHADER);
			const GLchar* psd = (GLchar*)ps_data->data;
			glShaderSource(_idfp, 1, &psd, nullptr);
			glCompileShader(_idfp);
			validate_shader(_idfp, (GLchar*)ps_data->data);
		}

		_id = glCreateProgram();
		if(vs_data != nullptr)
			glAttachShader(_id, _idvp);
		if(ps_data != nullptr)
			glAttachShader(_id, _idfp);
		
		glLinkProgram(_id);

		{
			char buf[512];
			GLsizei len = 0;
			glGetProgramInfoLog(_id, 512, &len, buf);
			if (len > 0)
			{
				OutputDebugString(L"GL Program error: ");
				OutputDebugStringA(buf);
				OutputDebugString(L"\n");
				throw exception(buf);
			}
			glValidateProgram(_id);
			GLint sta;
			glGetProgramiv(_id, GL_VALIDATE_STATUS, &sta);
			if (sta == GL_FALSE) throw exception("error validating shader");
		}

	}

	void shader::bind(device* _dev)
	{
		glUseProgram(_id);
	}

	void shader::unbind(device* _dev)
	{
		glUseProgram(0);
	}

	void shader::update(device* _dev)
	{
	}

	shader::~shader()
	{
		glDetachShader(_id, _idvp);
		glDetachShader(_id, _idfp);

		glDeleteShader(_idfp);
		glDeleteShader(_idvp);
		glDeleteProgram(_id);
	}
#endif
}
