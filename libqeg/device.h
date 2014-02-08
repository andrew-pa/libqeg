#pragma once
#include "cmmn.h"
//#include "render_texture2d.h"

namespace qeg
{
	class render_texture2d;
	class device
	{
		stack<render_texture2d*> rt_sk;
	protected:
#ifdef WIN32
#ifdef OPENGL
		HDC dc;
		HGLRC rc;
		vec2 _rtsize;
#endif
#ifdef DIRECTX	
	protected:		
		ComPtr<ID3D11Texture2D> backBuffer;
		ComPtr<ID3D11Texture2D> offscreenBackBuffer;
		ComPtr<ID3D11RenderTargetView> offscreenRenderTargetView;
		uint msaa_level;		
		D3D_FEATURE_LEVEL featureLevel;
		vec2 win_bnds;
		HWND _window;
		inline float convdp(float dips)
		{
			static const float c = 96.f;
			static const float logical_dpi = 96.f;
			return floor(dips * logical_dpi / c + 0.5f);
		}
		void create_device_res();
		void create_window_size_depres();
		void create_d2d_res();
		ComPtr<ID3D11Device2> _device;
		ComPtr<ID3D11DeviceContext2> _context;
		ComPtr<IDXGISwapChain1> _swap_chain;
		ComPtr<ID3D11RenderTargetView> render_target;
		ComPtr<ID3D11DepthStencilView> depth_stencil;

		ComPtr<ID2D1Factory1> _d2factory;
		ComPtr<IDWriteFactory1> _dwfactory;
		ComPtr<ID2D1Device> _d2device;
		ComPtr<ID2D1DeviceContext> _d2context;
		ComPtr<ID2D1Bitmap1> d2target_bitmap;
	public:
		propr(ComPtr<ID3D11Device2>, ddevice, { return _device; });
		propr(ComPtr<ID3D11DeviceContext2>, context, { return _context; });
		propr(ComPtr<IDXGISwapChain1>, swap_chain, { return _swap_chain; });
		propr(ComPtr<ID2D1DeviceContext>, d2context, { return _d2context; });
		propr(ComPtr<ID2D1Device>, d2device, { return _d2device; });
		propr(ComPtr<ID2D1Factory1>, d2factory, { return _d2factory; });
		propr(ComPtr<IDWriteFactory1>, dwfactory, { return _dwfactory; });


		propr(D3D_FEATURE_LEVEL, feature_lvl, { return featureLevel; });
		propr(HWND, window, { return _window; });
#endif
#endif
	public:
		device(){}
		device(vec2 _s 
#ifdef WIN32
			, HWND win_
#endif
#ifdef DIRECTX
			, uint msaa_lvl_ = 4
#endif
			);
		~device();
		inline render_texture2d* current_render_target() const;
		void pop_render_target();
		void push_render_target(render_texture2d* rt);
		void update_render_target();
		void present();
		void resize(vec2 ns);
		
#ifdef OPENGL
		propr(vec2, size, { return _rtsize; });
#elif DIRECTX
		propr(vec2, size, { return win_bnds; });
#endif
	};
};

