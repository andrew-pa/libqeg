#include "cmmn.h"
#include "device.h"
#include "render_texture2d.h"

namespace qeg
{
#ifdef WIN32
#ifdef DIRECTX
	device::device(vec2 s, HWND win_, uint msaa_lvl_)
		: msaa_level(msaa_lvl_), _window(win_), win_bnds(s)
	{
		create_d2d_res();
		create_device_res();
		create_window_size_depres();
	}

	void device::create_d2d_res()
	{
		D2D1_FACTORY_OPTIONS opt;
		ZeroMemory(&opt, sizeof(D2D1_FACTORY_OPTIONS));
#if defined(_DEBUG)
		opt.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
		chr(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &opt, &_d2factory));
		chr(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &_dwfactory));
	}

	void device::create_device_res()
	{
		UINT creflg = 0;
		creflg |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
		creflg |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		ComPtr<ID3D11Device> xdev;
		ComPtr<ID3D11DeviceContext> xcntx;
		chr(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creflg,
			featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
			&xdev, &featureLevel, &xcntx));

		chr(xdev.As(&_device));
		chr(xcntx.As(&_context));

		ComPtr<IDXGIDevice> dxgidevice;
		chr(_device.As(&dxgidevice));
		chr(_d2factory->CreateDevice(dxgidevice.Get(), &_d2device));
		chr(_d2device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &_d2context));
		
		CD3D11_RASTERIZER_DESC rsd(D3D11_FILL_SOLID, D3D11_CULL_BACK, TRUE, 0, 0, 0, TRUE, FALSE, FALSE, FALSE);
		_device->CreateRasterizerState(&rsd, &default_rsstate);
		_bind_default_rs_state();
	}

	void device::create_window_size_depres()
	{
		RECT cre;
		GetClientRect(_window, &cre);
		win_bnds = vec2(convdp((float)(cre.right - cre.left)), convdp((float)(cre.bottom - cre.top)));
		
		render_target.Reset();
		depth_stencil.Reset();

		rt_sk = stack<render_texture2d*>();

		if (msaa_level > 1)
			backBuffer.Reset();

		auto fmt = DXGI_FORMAT_B8G8R8A8_UNORM;

		if (_swap_chain != nullptr)
		{
			chr(_swap_chain->ResizeBuffers(2, (UINT)win_bnds.x, (UINT)win_bnds.y, fmt, 0));
		}
		else
		{
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
			swapChainDesc.Width = static_cast<UINT>(win_bnds.x);
			swapChainDesc.Height = static_cast<UINT>(win_bnds.y);
			swapChainDesc.Format = fmt;
			swapChainDesc.Stereo = false;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;
			swapChainDesc.Scaling = DXGI_SCALING_NONE;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			swapChainDesc.Flags = 0;

			ComPtr<IDXGIDevice2> dxgid;
			chr(_device.As(&dxgid));

			ComPtr<IDXGIAdapter> dxgiadp;
			chr(dxgid->GetAdapter(&dxgiadp));

			ComPtr<IDXGIFactory2> dxgif;
			chr(dxgiadp->GetParent(__uuidof(IDXGIFactory2), &dxgif));

			chr(dxgif->CreateSwapChainForHwnd(_device.Get(), _window, &swapChainDesc, nullptr, nullptr, &_swap_chain));

		}

#pragma region MSAA
		if (msaa_level > 1)
		{
			CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_B8G8R8A8_UNORM, (UINT)win_bnds.x, (UINT)win_bnds.y, 1, 1);
			desc.BindFlags = D3D11_BIND_RENDER_TARGET;
			desc.SampleDesc.Count = msaa_level;

			chr(
				_device->CreateTexture2D(
				&desc,
				nullptr,
				&offscreenBackBuffer
				)
				);

			chr(
				_device->CreateRenderTargetView(
				offscreenBackBuffer.Get(),
				nullptr,
				&offscreenRenderTargetView
				)
				);

			chr(_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));
			chr(_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &render_target));

			// Create a depth stencil view.
			CD3D11_TEXTURE2D_DESC depthStencilDesc(
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				static_cast<UINT>(win_bnds.x),
				static_cast<UINT>(win_bnds.y),
				1,
				1,
				D3D11_BIND_DEPTH_STENCIL
				);

			depthStencilDesc.SampleDesc.Count = msaa_level;

			ComPtr<ID3D11Texture2D> depthStencil;
			chr(
				_device->CreateTexture2D(
					&depthStencilDesc,
					nullptr,
					&depthStencil
				)
			);

			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);
			chr(
				_device->CreateDepthStencilView(
					depthStencil.Get(),
					&depthStencilViewDesc,
					&depth_stencil
					)
				);

			push_render_target(new render_texture2d(win_bnds, offscreenRenderTargetView, depth_stencil));
		}
#pragma endregion
#pragma region NO MSAA
		else
		{
			//ComPtr<ID3D11Texture2D> backBuffer;
			chr(_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));

			chr(_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &render_target));

			CD3D11_TEXTURE2D_DESC depthStencilDesc(
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				static_cast<UINT>(win_bnds.x),
				static_cast<UINT>(win_bnds.y),
				1,
				1,
				D3D11_BIND_DEPTH_STENCIL
				);

			ComPtr<ID3D11Texture2D> depthStencil;
			chr(_device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil));

			CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(D3D11_DSV_DIMENSION_TEXTURE2D);
			chr(_device->CreateDepthStencilView(depthStencil.Get(), &dsvd, &depth_stencil));

			push_render_target(new render_texture2d(win_bnds, render_target, depth_stencil));
		}
#pragma endregion
		
		vec2 dpi;
		_d2factory->GetDesktopDpi(&dpi.x, &dpi.y);
		

		D2D1_BITMAP_PROPERTIES1 bitprop =
			D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpi.x,
			dpi.y); 
		ComPtr<IDXGISurface> dxgibb;
if(msaa_level > 1)
		chr(offscreenBackBuffer.As(&dxgibb));
else
		chr(backBuffer.As(&dxgibb));
		chr(_d2context->CreateBitmapFromDxgiSurface(dxgibb.Get(), &bitprop, &d2target_bitmap));
		_d2context->SetTarget(d2target_bitmap.Get());
		_d2context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	}

	void device::resize(vec2 ns)
	{
		if (ns == win_bnds) return;
		ID3D11RenderTargetView* nullviews[] = { nullptr };
		_context->OMSetRenderTargets(1, nullviews, nullptr);
		_context->Flush();
		_d2context->SetTarget(nullptr);
		d2target_bitmap = nullptr;		
		vec2 dpi;
		_d2factory->GetDesktopDpi(&dpi.x, &dpi.y);
		_d2context->SetDpi(dpi.x, dpi.y);
		create_window_size_depres();
	}

	void device::present()
	{
		if (msaa_level > 1)
		{
			_context->ResolveSubresource(backBuffer.Get(), 0, offscreenBackBuffer.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
		}
		DXGI_PRESENT_PARAMETERS p = { 0 };
		auto h = _swap_chain->Present1(1, 0, &p);
		_context->DiscardView(render_target.Get());
		_context->DiscardView(depth_stencil.Get());
		if (h == DXGI_ERROR_DEVICE_REMOVED)
		{
			h = _device->GetDeviceRemovedReason();
			RECT cre;
			GetClientRect(_window, &cre);
			auto ns = vec2(convdp((float)(cre.right - cre.left)), convdp((float)(cre.bottom - cre.top)));
			_swap_chain = nullptr;
			create_device_res();
			create_d2d_res();
			resize(ns);
		}
		else chr(h);
	}

	void device::push_render_target(render_texture2d* rt)
	{
		rt_sk.push(rt);
		update_render_target();
	}

	void device::update_render_target()
	{
		auto r = rt_sk.top();
		const float clear_color[] = { 1.f, .5f, 0.f, 0.f };
		if (r->render_target() != nullptr)
			_context->ClearRenderTargetView(r->render_target().Get(), clear_color);
		if (r->depth_stencil() != nullptr)
			_context->ClearDepthStencilView(r->depth_stencil().Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
		_context->OMSetRenderTargets(1, r->render_target().GetAddressOf(), r->depth_stencil().Get());
		CD3D11_VIEWPORT vp(0.f, 0.f, (FLOAT)r->size().x, (FLOAT)r->size().y);
		_context->RSSetViewports(1, &vp);
	}

	render_texture2d* device::current_render_target() const
	{
		return rt_sk.top();
	}


	void device::pop_render_target()
	{
		if (rt_sk.size() == 1) return;
		rt_sk.pop();
		update_render_target();
	}

	device::~device()
	{
	}
#endif
#ifdef OPENGL

	static void debug_gl_callback(unsigned int source, unsigned int type,
		unsigned int id, unsigned int sev, int len, const char* msg, void* usr)
	{
		ostringstream oss;
		string ssource, stype, ssev;
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             ssource = "GL"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   ssource = "GUI"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: ssource = "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     ssource = "3rd Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     ssource = "App"; break;
		case GL_DEBUG_SOURCE_OTHER:           ssource = "Other"; break;
		}

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: stype = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: stype = "Decaprecated"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: stype = "Undefined"; break;
		case GL_DEBUG_TYPE_PORTABILITY: stype = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: stype = "Perf"; break;
		case GL_DEBUG_TYPE_OTHER: stype = "Other"; break;
		}

		switch (sev)
		{
		case GL_DEBUG_SEVERITY_HIGH: ssev = "H"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: ssev = "M"; break;
		case GL_DEBUG_SEVERITY_LOW: ssev = "L"; break;
		}

		oss << ssource << "> " << ssev << "[" << stype << "] (" << id << ") " << msg;
		if (sev == GL_DEBUG_SEVERITY_HIGH)
		{
			cdlog << "GL error: " + oss.str();
			OutputDebugStringA(("GL error: " + oss.str()).c_str());
			throw exception(("GL error: " + oss.str()).c_str());
		}
		else
			cdlog << "GL: " << oss.str();//OutputDebugStringA(oss.str().c_str());
	}

	device::device(vec2 _s, HWND win_)
		: dc(GetDC(win_)), _rtsize(_s), next_uniform_buffer_bind_index(0)
	{
		PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;

		auto pf = ChoosePixelFormat(dc, &pfd);
		if (pf == 0) throw exception("ChoosePixelFormat failed (Pixel format is invalid?)");
		if (!SetPixelFormat(dc, pf, &pfd)) throw exception("SetPixelFormat failed!");

		auto tempCtx = wglCreateContext(dc);
		wglMakeCurrent(dc, tempCtx);

		glewInit();
		int attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#ifdef _DEBUG
			| WGL_CONTEXT_DEBUG_BIT_ARB
#endif
			, // Set our OpenGL context to be forward compatible
			0
		};

		if (wglewIsSupported("WGL_ARB_create_context") == 1)
		{
			rc = wglCreateContextAttribsARB(dc, nullptr, attributes);
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(tempCtx);
			wglMakeCurrent(dc, rc);
		}
		else
		{
			throw exception("OpenGL 3.x+ context not supported!");
		}

#ifdef _DEBUG
		const GLubyte* glVersionString = glGetString(GL_VERSION);
		int glVersion[2] = { -1, -1 };
		glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
		glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
		cdlog << "GL Version: " << glVersion[0] << "." << glVersion[1] << endl;
		cdlog << "GL Vendor: " << glGetString(GL_VENDOR) << endl;
		cdlog << "GL Version String: " << glVersionString << endl;

		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageCallbackARB((GLDEBUGPROCARB)&debug_gl_callback, this);
#endif

		rt_sk.push(new render_texture2d(vec2(-1,-1), 0, 0));

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(1.f, .5f, 0.f, 0.f);
	}
	
	void device::present()
	{
		SwapBuffers(dc);
	}
	
	render_texture2d* device::current_render_target() const
	{
		return rt_sk.top();
	}
	
	void device::pop_render_target()
	{
		if (rt_sk.size() == 1) return;
		rt_sk.pop();
		update_render_target();
	}

	void device::push_render_target(render_texture2d* rt)
	{
		rt_sk.push(rt);
		update_render_target();
	}

	void device::update_render_target()
	{	
		glBindFramebuffer(GL_FRAMEBUFFER, rt_sk.top()->frame_buffer());
		if (rt_sk.top()->size().x == -1)
			glViewport(0, 0, (GLsizei)size().x, (GLsizei)size().y);
		else
			glViewport(0, 0, (GLsizei)rt_sk.top()->size().x, (GLsizei)rt_sk.top()->size().y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	void device::resize(vec2 ns)
	{
		_rtsize = ns;
	}

	uint device::alloc_ubbi()
	{
		if(!previously_alloced_ubbi.empty())
		{
			auto x = previously_alloced_ubbi.front();
			previously_alloced_ubbi.pop();
			return x;
		}
		return next_uniform_buffer_bind_index++;
	}

	void device::free_ubbi(uint& ubbi)
	{
		previously_alloced_ubbi.push(ubbi);
		ubbi = -1;
	}

	device::~device()
	{
	}
#endif
#endif
};