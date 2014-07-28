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

	shader::shader(device* _dev, const datablob<byte>& vs_data, const datablob<byte>& ps_data, 
		const D3D11_INPUT_ELEMENT_DESC lo[], size_t cnt)
	{
		if (!vs_data.empty())
			chr(_dev->ddevice()->CreateVertexShader(vs_data.data, vs_data.length, nullptr, &vertex_sh));
		if (!ps_data.empty())
			chr(_dev->ddevice()->CreatePixelShader(ps_data.data, ps_data.length, nullptr, &pixel_sh));
		chr(_dev->ddevice()->CreateInputLayout(lo, cnt, vs_data.data, vs_data.length, &inplo));
	}	
	
	shader::shader(device* _dev, const datablob<byte>& vs_data, const datablob<byte>& ps_data, const datablob<byte>& gs_data)
	{
		if (!vs_data.empty())
			chr(_dev->ddevice()->CreateVertexShader(vs_data.data, vs_data.length, nullptr, &vertex_sh));

		if (!gs_data.empty())
			chr(_dev->ddevice()->CreateGeometryShader(gs_data.data, gs_data.length, nullptr, &geom_sh));

		if (!ps_data.empty())
			chr(_dev->ddevice()->CreatePixelShader(ps_data.data, ps_data.length, nullptr, &pixel_sh));

		ID3D11ShaderReflection* vsref;
		chr(D3DReflect(vs_data.data, vs_data.length,
			IID_ID3D11ShaderReflection, (void**)&vsref));
		D3D11_SHADER_DESC shd;
		vsref->GetDesc(&shd);

		vector<D3D11_INPUT_ELEMENT_DESC> eled;
		for (uint i = 0; i < shd.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC pd;
			vsref->GetInputParameterDesc(i, &pd);

			D3D11_INPUT_ELEMENT_DESC ed;
			ed.SemanticName = pd.SemanticName;
			ed.SemanticIndex = pd.SemanticIndex;
			ed.InputSlot = 0;
			ed.AlignedByteOffset = i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
			ed.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			ed.InstanceDataStepRate = 0;

			if (pd.Mask == 1)
			{
				if (pd.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  ed.Format = DXGI_FORMAT_R32_UINT;
				else if (pd.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  ed.Format = DXGI_FORMAT_R32_SINT;
				else if (pd.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ed.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (pd.Mask <= 3)
			{
				if (pd.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  ed.Format = DXGI_FORMAT_R32G32_UINT;
				else if (pd.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  ed.Format = DXGI_FORMAT_R32G32_SINT;
				else if (pd.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ed.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (pd.Mask <= 7)
			{
				if (pd.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  ed.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (pd.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  ed.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (pd.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ed.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (pd.Mask <= 15)
			{
				if (pd.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  ed.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (pd.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  ed.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (pd.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ed.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			eled.push_back(ed);
		}

		chr(_dev->ddevice()->CreateInputLayout(eled.data(), eled.size(), vs_data.data, vs_data.length, &inplo));

		vsref->Release();
	}

	void shader::bind(device* _dev)
	{
		if(vertex_sh) _dev->context()->VSSetShader(vertex_sh.Get(), nullptr, 0);
		if (geom_sh)  _dev->context()->GSSetShader(geom_sh.Get(), nullptr, 0);
		if(pixel_sh)  _dev->context()->PSSetShader(pixel_sh.Get(), nullptr, 0);
		_dev->context()->IASetInputLayout(inplo.Get());
	}

	void shader::unbind(device* _dev)
	{
		_dev->context()->VSSetShader(nullptr, nullptr, 0);
		_dev->context()->GSSetShader(nullptr, nullptr, 0);
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
		//OutputDebugStringA(fn.c_str());
		glGetShaderInfoLog(shader, 512, &len, buf);
		if (len > 0)
		{
			ostringstream oss;
			oss << "Shader " << shader << " (" << fn << ")" << " error: " << buf;
			cdlog << "Shader Compile Error:: " << oss.str() << endl;
			//OutputDebugStringA(buf);
			throw exception(oss.str().c_str());
		}
	}

	static GLuint compile_shader(GLenum type, const datablob<byte>& fdata, const string& defs)
	{
		GLuint sh = glCreateShader(type);
		string vssd = string((char*)fdata.data, fdata.length + (char*)fdata.data);
		auto tw = vssd.find_first_of("~~~~", 0);
		if (tw != vssd.npos) 
		{
			vssd.replace(tw, 4, "");
			vssd.insert(tw, defs);
		}
		const GLchar* vsd = (GLchar*)vssd.data();
		const GLint vsl = vssd.size();
		//OutputDebugStringA(vsd);
		glShaderSource(sh, 1, &vsd, &vsl);
		glCompileShader(sh);
		validate_shader(sh, vsd);
		return sh;
	}

	shader::shader(device* _dev, const datablob<byte>& vs_data, const datablob<byte>& ps_data, const datablob<byte>& gs_data)
	{
		string defs = "";
		if (!gs_data.empty()) defs += "#define IN_FROM_GS";
		if(!vs_data.empty())
		{
			/*_idvp = glCreateShader(GL_VERTEX_SHADER);
			string vssd = string((char*)vs_data.data, vs_data.length+(char*)vs_data.data);	
			auto tw = vssd.find_first_of("~~~~", 0);
			if(tw != vssd.npos) {vssd.replace(tw, 4, "");
			vssd.insert(tw, defs);}
			const GLchar* vsd = (GLchar*)vssd.data();
			const GLint vsl = vssd.size();
			//OutputDebugStringA(vsd);
			glShaderSource(_idvp, 1, &vsd, &vsl);
			glCompileShader(_idvp);
			validate_shader(_idvp, vsd);*/
			_idvp = compile_shader(GL_VERTEX_SHADER, vs_data, defs);
		}

		if(!ps_data.empty())
		{			
			/*_idfp = glCreateShader(GL_FRAGMENT_SHADER);
			const GLchar* psd = (GLchar*)ps_data.data;
			const GLint psl = ps_data.length;
			glShaderSource(_idfp, 1, &psd, &psl);
			glCompileShader(_idfp);
			validate_shader(_idfp, psd);*/
			_idfp = compile_shader(GL_FRAGMENT_SHADER, ps_data, defs);
		}

		if (!gs_data.empty())
		{
			/*_idgp = glCreateShader(GL_GEOMETRY_SHADER);
			const GLchar* gsd = (GLchar*)gs_data.data;
			const GLint gsl = gs_data.length;
			glShaderSource(_idgp, 1, &gsd, &gsl);
			glCompileShader(_idgp);
			validate_shader(_idgp, gsd);*/
			_idgp = compile_shader(GL_GEOMETRY_SHADER, gs_data, defs);
		}


		_id = glCreateProgram();
		if(!vs_data.empty())
			glAttachShader(_id, _idvp);
		if(!ps_data.empty())
			glAttachShader(_id, _idfp);
		if (!gs_data.empty())
			glAttachShader(_id, _idgp);

		glLinkProgram(_id);

		{
			char buf[512];
			GLsizei len = 0;
			glGetProgramInfoLog(_id, 512, &len, buf);
			if (len > 0)
			{
				cdlog << "GL Program error: " << buf << endl;
				throw exception(buf);
			}
			glValidateProgram(_id);
			GLint sta;
			glGetProgramiv(_id, GL_VALIDATE_STATUS, &sta);
			//if (sta == GL_FALSE) throw exception("error validating shader");
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
		glDetachShader(_id, _idgp);
		glDetachShader(_id, _idfp);

		glDeleteShader(_idfp);
		glDeleteShader(_idvp);
		glDeleteShader(_idgp);
		glDeleteProgram(_id);
	}
#endif
}
