#pragma once
#include "cmmn.h"
//#include "render_target.h"

namespace qeg
{
	class render_target;
	class render_texture2d;
	class device
	{
		stack<render_target*> rt_sk;
		pixel_format f, df;
	protected:
#ifdef WIN32
#ifdef OPENGL
		//HDC dc;
		//HGLRC rc;
		
		GLFWwindow* wnd;
		
		vec2 _rtsize;

		uint next_uniform_buffer_bind_index;
		queue<uint> previously_alloced_ubbi;
		uint alloc_ubbi();
		void free_ubbi(uint& ubbi);
		template<typename T>
		friend class constant_buffer; //so can alloc/free ubbis
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
		render_texture2d* _back_buffer;

		ComPtr<ID2D1Factory1> _d2factory;
		ComPtr<IDWriteFactory1> _dwfactory;
		ComPtr<ID2D1Device> _d2device;
		ComPtr<ID2D1DeviceContext> _d2context;
		ComPtr<ID2D1Bitmap1> d2target_bitmap;

		ComPtr<ID3D11RasterizerState> default_rsstate;

		void _bind_default_rs_state()
		{
			_context->RSSetState(default_rsstate.Get());
		}
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
		device(vec2 _s , void* win_, uint aa_lvl_ = 4,
			pixel_format f = pixel_format::BGRA8_UNORM, pixel_format df = pixel_format::D32_FLOAT);
		~device();
		inline render_target* current_render_target() const;
		void pop_render_target();
		void push_render_target(render_target* rt);
		void update_render_target();
		void present();
		void resize(vec2 ns);
		
#ifdef OPENGL
		propr(vec2, size, { return _rtsize; });
#elif DIRECTX
		propr(vec2, size, { return win_bnds; });
		friend struct rasterizer_state; //for _bind_default_rs_state
		friend class default_render_target; //so that default_render_target can access _back_buffer render_texture2d 
#endif
	};
};

