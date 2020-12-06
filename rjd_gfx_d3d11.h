#pragma once

#define RJD_GFX_D3D11_H 1

#if !RJD_GFX_H
	#error "This header should only be included by rjd_gfx.h"
#endif

#if !RJD_IMPL
	#error "This should have only been included when RJD_IMPL is on"
#endif

#if !RJD_PLATFORM_WINDOWS
	#error "DirectX11 is only supported on Windows"
#endif

// This forward declaration is needed to suppress an order of declarations bug in the d3d headers.
// d3d11shader.h(454): warning C4115: 'ID3D11ModuleInstance': named type definition in parentheses
struct ID3D11ModuleInstance;

#define CINTERFACE
#define COBJMACROS
#include <d3d11_1.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>

#undef CINTERFACE
#undef COBJMACROS

////////////////////////////////////////////////////////////////////////////////
// Local helpers

struct rjd_gfx_texture_d3d11
{
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* resource_view;
	ID3D11SamplerState* sampler;
	struct rjd_strref* debug_name;
};

struct rjd_gfx_shader_d3d11
{
	ID3DBlob* bytecode;
	ID3D11VertexShader* vertex;
	ID3D11PixelShader* pixel;
	struct rjd_strref* debug_name;
	struct rjd_strref* debug_source;
};

struct rjd_gfx_pipeline_state_d3d11
{
	ID3D11InputLayout* vertex_layout;
	ID3D11RasterizerState* rasterizer_state;
	ID3D11DepthStencilState* depthstencil_state;
	struct rjd_gfx_shader shader_vertex;
	struct rjd_gfx_shader shader_pixel;
	struct rjd_gfx_texture render_target;
	struct rjd_gfx_texture depthstencil_target;
	enum rjd_gfx_depth_compare depth_compare;
	struct rjd_strref* debug_name;
};

struct rjd_gfx_mesh_buffer_d3d11
{
	ID3D11Buffer* buffer;
	UINT stride;
	UINT offset;
	UINT slot;
	enum rjd_gfx_mesh_buffer_usage_flags usage_flags;
};

struct rjd_gfx_mesh_d3d11
{
	struct rjd_gfx_mesh_buffer_d3d11* buffers;
	uint32_t count_buffers;
	uint32_t count_vertices;
	enum rjd_gfx_primitive_type primitive;
};

struct rjd_gfx_command_buffer_d3d11
{
	ID3D11DeviceContext1* deferred_context;
	ID3D11RenderTargetView* render_target_view;
	ID3D11DepthStencilView* depthstencil_view;
};

struct rjd_gfx_context_d3d11
{
	IDXGIFactory4* factory;
	IDXGIAdapter1* adapter;
	ID3D11Device1* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain1* swapchain;

	struct rjd_gfx_texture_d3d11* slotmap_textures;
	struct rjd_gfx_shader_d3d11* slotmap_shaders;
	struct rjd_gfx_pipeline_state_d3d11* slotmap_pipeline_states;
	struct rjd_gfx_mesh_d3d11* slotmap_meshes;
	struct rjd_gfx_command_buffer_d3d11* slotmap_command_buffers;

	ID3D11DeviceContext1** free_deferred_contexts;
	ID3D11Texture2D** depthbuffers;
	ID3D11Texture2D** msaa_depthbuffers;
	struct rjd_gfx_texture* msaa_backbuffers;

	struct rjd_mem_allocator* allocator;
	struct rjd_strpool debug_names;
	HWND hwnd;
	bool is_occluded;
	uint8_t num_backbuffers;
	uint8_t backbuffer_index;
	enum rjd_gfx_format backbuffer_color_format;
	enum rjd_gfx_format backbuffer_depth_format;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_gfx_context_d3d11) <= sizeof(struct rjd_gfx_context));

struct rjd_gfx_rgba
{
	float v[4];
};

static struct rjd_result rjd_gfx_translate_hresult(HRESULT hr);

static DXGI_FORMAT rjd_gfx_format_to_dxgi(enum rjd_gfx_format format);
static DXGI_FORMAT rjd_gfx_format_to_dxgi_strip_srgb(enum rjd_gfx_format format);
static struct rjd_gfx_rgba rjd_gfx_format_value_to_rgba(struct rjd_gfx_format_value value);
static D3D_PRIMITIVE_TOPOLOGY rjd_gfx_primitive_to_d3d11(enum rjd_gfx_primitive_type primitive);
static D3D11_CULL_MODE rjd_gfx_cull_to_d3d(enum rjd_gfx_cull cull_mode);
static D3D11_COMPARISON_FUNC rjd_gfx_depth_compare_to_d3d11(enum rjd_gfx_depth_compare compare);
static D3D11_USAGE rjd_gfx_texture_access_to_gpu_access(enum rjd_gfx_texture_access access);
static D3D11_CPU_ACCESS_FLAG rjd_gfx_texture_access_to_cpu_access(enum rjd_gfx_texture_access access);
static const char* rjd_gfx_semantic_to_name(enum rjd_gfx_vertex_semantic semantic);

static bool rjd_gfx_texture_isbackbuffer(struct rjd_gfx_texture texture);

static struct rjd_result rjd_gfx_backbuffer_get_msaa_quality_d3d11(const struct rjd_gfx_context_d3d11* context, uint32_t sample_count, DXGI_SAMPLE_DESC* out);

static inline void rjd_gfx_texture_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot);
static inline void rjd_gfx_shader_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot);
static inline void rjd_gfx_pipeline_state_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot);
static inline void rjd_gfx_mesh_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot);
static inline void rjd_gfx_command_buffer_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot);

////////////////////////////////////////////////////////////////////////////////
// Local data

#if RJD_COMPILER_GCC
	// The GCC headers have all the correct declarations for these GUIDs, but libdxguid.a are missing the definitions
	const GUID IID_IDXGIFactory4 = { 0x1bc6ea02, 0xef36, 0x464f, { 0xbf,0x0c,0x21,0xca,0x39,0xe5,0x16,0x8a } };
#endif

////////////////////////////////////////////////////////////////////////////////
// interface implementation

struct rjd_result rjd_gfx_context_create(struct rjd_gfx_context* out, struct rjd_gfx_context_desc desc)
{
	RJD_ASSERT(out);

	IDXGIFactory4* factory = NULL;
	{
		UINT flags = 0;
		flags |= DXGI_CREATE_FACTORY_DEBUG; // TODO make this optional

		HRESULT hr = CreateDXGIFactory2(flags, &IID_IDXGIFactory4, (void**)&factory);
		if (!SUCCEEDED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}
	}

	IDXGIAdapter1* adapter_extended = NULL;
	struct rjd_result result_adapter = RJD_RESULT("No hardware adapter found. Does this machine have a GPU?");
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != IDXGIFactory1_EnumAdapters1(factory, i, &adapter_extended); ++i) {
		DXGI_ADAPTER_DESC1 desc_adapter;
		IDXGIAdapter1_GetDesc1(adapter_extended, &desc_adapter);
		if (desc_adapter.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			adapter_extended = NULL;
			continue;
		}

		// desc_adapter.DedicatedVideoMemory
		// desc_adapter.Description // wchar_t* name
		result_adapter = RJD_RESULT_OK();
		break;
	}
	if (!rjd_result_isok(result_adapter)) {
		return result_adapter;
	}

	//IDXGIAdapter* adapter = NULL;
	//if (adapter_extended) {
	//	IDXGIAdapter1_QueryInterface(adapter_extended, &IID_IDXGIAdapter, &adapter);
	//}

	ID3D11Device1* device = NULL;
	ID3D11DeviceContext* context = NULL;
	{
		const UINT flags =
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | // enable compatibility with Direct2D.
			D3D11_CREATE_DEVICE_DEBUG | // TODO make this optional
			0;

		const D3D_FEATURE_LEVEL feature_levels[] = {
			D3D_FEATURE_LEVEL_11_1,
		};

		ID3D11Device* device_11_0 = NULL;
		const HRESULT hr_device = D3D11CreateDevice(
			//(IDXGIAdapter*)adapter, // TODO verify this cast is legit
			NULL, // TODO replace this with the specified adapter
			D3D_DRIVER_TYPE_HARDWARE,
			NULL, // not using a software rasterizer
			flags,
			feature_levels,
			rjd_countof(feature_levels),
			D3D11_SDK_VERSION,
			&device_11_0,
			NULL,
			&context);

		if (FAILED(hr_device)) {
			if (hr_device == DXGI_ERROR_SDK_COMPONENT_MISSING) {
				if (flags & D3D11_CREATE_DEVICE_DEBUG) {
					return RJD_RESULT("Correct debug layer version not available on this machine. "
						"Make sure the DirectX SDK is up to date.");
				}
				else {
					return RJD_RESULT("A SDK component was not available.");
				}
			}
			else {
				return rjd_gfx_translate_hresult(hr_device);
			}
		}

		ID3D11Device_QueryInterface(device_11_0, &IID_ID3D11Device1, &device);
		ID3D11Device_Release(device_11_0);
		if (device == NULL) {
			return RJD_RESULT("Unable to create 11.1 device. 11.0 is not supported.");
		}
	}

	// DXGI doesn't support SRGB formats in the swapchain, but if the user wants one, we can still fake it
	// by using SRGB views.
	DXGI_FORMAT backbuffer_format = rjd_gfx_format_to_dxgi(desc.backbuffer_color_format);
	{
		if (backbuffer_format != DXGI_FORMAT_B8G8R8A8_UNORM &&
			backbuffer_format != DXGI_FORMAT_R8G8B8A8_UNORM &&
			backbuffer_format != DXGI_FORMAT_R16G16B16A16_FLOAT &&
			backbuffer_format != DXGI_FORMAT_R10G10B10A2_UNORM) {
			backbuffer_format = rjd_gfx_format_to_dxgi_strip_srgb(desc.backbuffer_color_format);
		}
		if (backbuffer_format != DXGI_FORMAT_B8G8R8A8_UNORM &&
			backbuffer_format != DXGI_FORMAT_R8G8B8A8_UNORM &&
			backbuffer_format != DXGI_FORMAT_R16G16B16A16_FLOAT &&
			backbuffer_format != DXGI_FORMAT_R10G10B10A2_UNORM) {
			return RJD_RESULT("The backbuffer format must be one of these: "
								"DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R10G10B10A2_UNORM");
		}
	}

	if (desc.num_backbuffers > 3) {
		return RJD_RESULT("You specified more than 3 framebuffers Only 1-3 are supported.");
	}
	const UINT num_backbuffers = (desc.num_backbuffers == 0) ? 3 : desc.num_backbuffers;

	IDXGISwapChain1* swapchain = NULL;
	{
		DXGI_RATIONAL refresh_rate = {
			.Numerator = 0,
			.Denominator = 1,
		};

		// note that the flip model isn't compatible with msaa so we'll have to render to an offscreen msaa
		// target to get the same effect
		DXGI_SAMPLE_DESC desc_msaa = {
			.Count = 1,
			.Quality = 0,
		};

		uint32_t flags = 0;
		flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // needed to switch between fullscreen/windowed
		flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING; // needed to turn off vsync

		DXGI_SWAP_CHAIN_DESC1 desc_swapchain = {
			.Width = 0, // auto-detect
			.Height = 0, // auto-detect
			.Format = backbuffer_format,
			.Stereo = false,
			.SampleDesc = desc_msaa,
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = num_backbuffers,
			.Scaling = DXGI_SCALING_STRETCH, // Don't stretch the backbuffer to fit the window size. We should handle that. // TODO figure out why DXGI_SCALING_NONE fails
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD, // flip model has better perf and discard since we don't care to keep it around
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = flags,
		};

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC desc_fullscreen = {
			.RefreshRate = refresh_rate,
			.Windowed = TRUE, // TODO
		};

		HRESULT hr_swap = IDXGIFactory2_CreateSwapChainForHwnd(
			factory,
			(IUnknown*)device,
			desc.win32.hwnd,
			&desc_swapchain,
			&desc_fullscreen,
			NULL, // don't restrict to a particular output of the adapter
			&swapchain);
		if (FAILED(hr_swap)) {
			switch (hr_swap) {
				case E_OUTOFMEMORY: return RJD_RESULT("Failed to create swapchain: out of memory");
				case DXGI_ERROR_INVALID_CALL: RJD_ASSERTFAIL("We should detect this before it gets here."); break;
				default: return RJD_RESULT("Failed to create swapchain.");
			}
		}
	}

	// Create backing depth buffers
	ID3D11Texture2D** depthbuffers = rjd_mem_alloc_array(ID3D11Texture2D*, num_backbuffers, desc.allocator);
	{
		DXGI_FORMAT depth_format_dxgi = rjd_gfx_format_to_dxgi(desc.backbuffer_depth_format);

		DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {0};
		HRESULT hr_swap = IDXGISwapChain1_GetDesc1(swapchain, &swapchain_desc);
		if (FAILED(hr_swap)) {
			return RJD_RESULT("Failed to get swapchain desc to get auto-detected backbuffer size.");
		}

		D3D11_TEXTURE2D_DESC desc_depthbuffer = {
			.Width = swapchain_desc.Width,
			.Height = swapchain_desc.Height,
			.MipLevels = 1,
			.ArraySize = 1,
			.Format = depth_format_dxgi,
			.SampleDesc.Count = 1,
			.SampleDesc.Quality = 0,
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_DEPTH_STENCIL,
			.CPUAccessFlags = 0,
			.MiscFlags = 0,
		};

		for (size_t i = 0; i < num_backbuffers; ++i) {
			HRESULT hr = ID3D11Device_CreateTexture2D(device, &desc_depthbuffer, NULL, depthbuffers + i);
			if (FAILED(hr)) {
				return rjd_gfx_translate_hresult(hr);
			}
		}
	}

	memset(out, 0, sizeof(*out));
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)out;

	context_d3d11->slotmap_textures			= rjd_slotmap_alloc(struct rjd_gfx_texture_d3d11, 64, desc.allocator);
	context_d3d11->slotmap_shaders			= rjd_slotmap_alloc(struct rjd_gfx_shader_d3d11, 64, desc.allocator);
	context_d3d11->slotmap_pipeline_states	= rjd_slotmap_alloc(struct rjd_gfx_pipeline_state_d3d11, 64, desc.allocator);
	context_d3d11->slotmap_meshes			= rjd_slotmap_alloc(struct rjd_gfx_mesh_d3d11, 64, desc.allocator);
	context_d3d11->slotmap_command_buffers	= rjd_slotmap_alloc(struct rjd_gfx_command_buffer_d3d11, 16, desc.allocator);

	context_d3d11->free_deferred_contexts = rjd_array_alloc(ID3D11DeviceContext1*, 1, desc.allocator);
	context_d3d11->depthbuffers = depthbuffers;
	context_d3d11->msaa_backbuffers = NULL;

	context_d3d11->factory = factory;
	context_d3d11->adapter = adapter_extended;
	context_d3d11->device = device;
	context_d3d11->context = context;
	context_d3d11->swapchain = swapchain;

	context_d3d11->allocator = desc.allocator;
	context_d3d11->debug_names = rjd_strpool_init(desc.allocator, 128);
	context_d3d11->hwnd = desc.win32.hwnd;
	context_d3d11->is_occluded = false;
	context_d3d11->num_backbuffers = (uint8_t)num_backbuffers;
	context_d3d11->backbuffer_index = 0;
	context_d3d11->backbuffer_color_format = desc.backbuffer_color_format;
	context_d3d11->backbuffer_depth_format = desc.backbuffer_depth_format;

	return RJD_RESULT_OK();
}

void rjd_gfx_context_destroy(struct rjd_gfx_context* context)
{
	RJD_ASSERT(context);
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	struct rjd_gfx_texture_d3d11* textures = context_d3d11->slotmap_textures;
	struct rjd_gfx_shader_d3d11* shaders = context_d3d11->slotmap_shaders;
	struct rjd_gfx_pipeline_state_d3d11* states = context_d3d11->slotmap_pipeline_states;
	struct rjd_gfx_mesh_d3d11* meshes = context_d3d11->slotmap_meshes;
	struct rjd_gfx_command_buffer_d3d11* commands = context_d3d11->slotmap_command_buffers;

	for (struct rjd_slot s = rjd_slotmap_next(textures, NULL); rjd_slot_isvalid(s); s = rjd_slotmap_next(textures, &s)) {
		rjd_gfx_texture_destroy_d3d11(context_d3d11, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(shaders, NULL); rjd_slot_isvalid(s); s = rjd_slotmap_next(shaders, &s)) {
		rjd_gfx_shader_destroy_d3d11(context_d3d11, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(states, NULL); rjd_slot_isvalid(s); s = rjd_slotmap_next(states, &s)) {
		rjd_gfx_pipeline_state_destroy_d3d11(context_d3d11, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(meshes, NULL); rjd_slot_isvalid(s); s = rjd_slotmap_next(meshes, &s)) {
		rjd_gfx_mesh_destroy_d3d11(context_d3d11, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(commands, NULL); rjd_slot_isvalid(s); s = rjd_slotmap_next(commands, &s)) {
		rjd_gfx_command_buffer_destroy_d3d11(context_d3d11, s);
	}

	rjd_slotmap_free(textures);
	rjd_slotmap_free(shaders);
	rjd_slotmap_free(states);
	rjd_slotmap_free(meshes);
	rjd_slotmap_free(commands);

	rjd_strpool_free(&context_d3d11->debug_names);

	for (size_t i = 0; i < rjd_array_count(context_d3d11->free_deferred_contexts); ++i) {
		ID3D11DeviceContext1_Release(context_d3d11->free_deferred_contexts[i]);
	}
	rjd_array_free(context_d3d11->free_deferred_contexts);

	for (size_t i = 0; i < context_d3d11->num_backbuffers; ++i) {
		ID3D11Texture2D_Release(context_d3d11->depthbuffers[i]);
		if (context_d3d11->msaa_depthbuffers) {
			ID3D11Texture2D_Release(context_d3d11->msaa_depthbuffers[i]);
		}
	}
	rjd_mem_free(context_d3d11->depthbuffers);
	rjd_mem_free(context_d3d11->msaa_depthbuffers);

	// freeing these textures got taken care of in the above resource cleanup
	rjd_mem_free(context_d3d11->msaa_backbuffers);

	IDXGISwapChain1_Release(context_d3d11->swapchain);
	ID3D11DeviceContext_Release(context_d3d11->context);
	ID3D11Device_Release(context_d3d11->device);
	IDXGIAdapter1_Release(context_d3d11->adapter);
	IDXGIFactory4_Release(context_d3d11->factory);

#if RJD_COMPILER_MSVC // GCC doesn't know about IDXGIDebug1
	{
		IDXGIDebug1* debug = NULL;
		HRESULT hr = DXGIGetDebugInterface1(0, &IID_IDXGIDebug, &debug);
		if (SUCCEEDED(hr)) {
			IDXGIDebug_ReportLiveObjects(debug, DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
		}
	}
#endif
}

uint32_t rjd_gfx_backbuffer_current_index(const struct rjd_gfx_context* context)
{
	RJD_ASSERT(context);

	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	return context_d3d11->backbuffer_index;
}

struct rjd_result rjd_gfx_backbuffer_msaa_is_count_supported(const struct rjd_gfx_context* context, uint32_t sample_count)
{
	RJD_ASSERT(context);

	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	DXGI_SAMPLE_DESC unused = {0};
	return rjd_gfx_backbuffer_get_msaa_quality_d3d11(context_d3d11, sample_count, &unused);
}

struct rjd_result rjd_gfx_backbuffer_set_msaa_count(struct rjd_gfx_context* context, uint32_t sample_count)
{
	RJD_ASSERT(context);

	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	DXGI_SAMPLE_DESC sample_desc = {0};
	{
		struct rjd_result result = rjd_gfx_backbuffer_get_msaa_quality_d3d11(context_d3d11, sample_count, &sample_desc);
		if (!rjd_result_isok(result)) {
			return result;
		}
	}

	DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {0};
	HRESULT hr_swap = IDXGISwapChain1_GetDesc1(context_d3d11->swapchain, &swapchain_desc);
	if (FAILED(hr_swap)) {
		return RJD_RESULT("Failed to get swapchain desc to check MSAA compatibility.");
	}

	if (sample_desc.Count != swapchain_desc.SampleDesc.Count) {
		const uint32_t num_msaa_backbuffers = (sample_count > 1) ? context_d3d11->num_backbuffers : 0;

		if (context_d3d11->msaa_backbuffers) {
			for (uint32_t i = 0; i < context_d3d11->num_backbuffers; ++i) {
				rjd_gfx_texture_destroy(context, context_d3d11->msaa_backbuffers + i);
				ID3D11Texture2D_Release(context_d3d11->msaa_depthbuffers[i]);
			}

			rjd_mem_free(context_d3d11->msaa_backbuffers);
			rjd_mem_free(context_d3d11->msaa_depthbuffers);
			context_d3d11->msaa_backbuffers = NULL;
			context_d3d11->msaa_depthbuffers = NULL;
		}

		if (num_msaa_backbuffers > 0) {
			// color buffers
			context_d3d11->msaa_backbuffers = rjd_mem_alloc_array(struct rjd_gfx_texture, num_msaa_backbuffers, context_d3d11->allocator);
			for (size_t i = 0; i < num_msaa_backbuffers; ++i) {
				struct rjd_gfx_texture_desc desc = {
					.debug_label = "msaa_backbuffer_color",
					.data = NULL, 
					.data_length = 0,
					.pixels_width = swapchain_desc.Width,
					.pixels_height = swapchain_desc.Height,
					.msaa_samples = sample_count,
					.format = context_d3d11->backbuffer_color_format,
					.access = RJD_GFX_TEXTURE_ACCESS_CPU_NONE_GPU_READWRITE,
					.usage = RJD_GFX_TEXTURE_USAGE_RENDERTARGET,
				};

				struct rjd_result result = rjd_gfx_texture_create(context, context_d3d11->msaa_backbuffers + i, desc);
				if (!rjd_result_isok(result)) {
					return result;
				}
			}

			// msaa depthbuffers
			context_d3d11->msaa_depthbuffers = rjd_mem_alloc_array(ID3D11Texture2D*, num_msaa_backbuffers, context_d3d11->allocator);
			for (size_t i = 0; i < num_msaa_backbuffers; ++i) {
				DXGI_FORMAT depth_format_dxgi = rjd_gfx_format_to_dxgi(context_d3d11->backbuffer_depth_format);

				D3D11_TEXTURE2D_DESC desc_depthbuffer = {
					.Width = swapchain_desc.Width,
					.Height = swapchain_desc.Height,
					.MipLevels = 1,
					.ArraySize = 1,
					.Format = depth_format_dxgi,
					.SampleDesc = sample_desc,
					.Usage = D3D11_USAGE_DEFAULT,
					.BindFlags = D3D11_BIND_DEPTH_STENCIL,
					.CPUAccessFlags = 0,
					.MiscFlags = 0,
				};

				HRESULT hr = ID3D11Device_CreateTexture2D(context_d3d11->device, &desc_depthbuffer, NULL, context_d3d11->msaa_depthbuffers + i);
				if (FAILED(hr)) {
					return rjd_gfx_translate_hresult(hr);
				}
			}
		}
	}

	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_vsync_set(struct rjd_gfx_context* context, enum RJD_GFX_VSYNC_MODE mode)
{
	RJD_UNUSED_PARAM(context);
	RJD_UNUSED_PARAM(mode);
	return RJD_RESULT("unimplemented");
}

struct rjd_result rjd_gfx_wait_for_frame_begin(struct rjd_gfx_context* context)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	RECT rect_window = {0};
	if (GetWindowRect(context_d3d11->hwnd, &rect_window) == FALSE) {
		return RJD_RESULT("Failed to get window rect. Check GetLastError() for more information");
	}

	IDXGIOutput* output = NULL;
	IDXGIOutput* selected_output = NULL;
	uint32_t selected_intersect_area = 0;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != IDXGIAdapter1_EnumOutputs(context_d3d11->adapter, i, &output); ++i) {
		DXGI_OUTPUT_DESC desc_output = {0};
		HRESULT hr = IDXGIOutput_GetDesc(output, &desc_output);
		if (SUCCEEDED(hr)) {
			RECT rect_intersect = {0};
			if (IntersectRect(&rect_intersect, &rect_window, &desc_output.DesktopCoordinates)) {
				uint32_t output_intersect_area = (rect_intersect.right - rect_intersect.left) * (rect_intersect.bottom - rect_intersect.top);

				if (selected_output == NULL || output_intersect_area > selected_intersect_area) {
					selected_output = output;
					selected_intersect_area = output_intersect_area;
				}
			}
		}
	}

	if (selected_output == NULL) {
		return RJD_RESULT("No outputs found. Were all monitors unplugged?");
	}

	HRESULT hr = IDXGIOutput_WaitForVBlank(selected_output);

	IDXGIOutput_Release(selected_output);

	struct rjd_result result = rjd_gfx_translate_hresult(hr);
	return result;
}

struct rjd_result rjd_gfx_present(struct rjd_gfx_context* context)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	const bool USE_VSYNC = true; // TODO make this configurable

	UINT sync_interval = 0;
	UINT flags = 0;
	if (USE_VSYNC) {
		sync_interval = 1;
		flags = DXGI_PRESENT_DO_NOT_WAIT;
	} else {
		sync_interval = 0;
		flags = DXGI_PRESENT_DO_NOT_WAIT | DXGI_PRESENT_ALLOW_TEARING;
	}

	if (context_d3d11->is_occluded) {
		flags |= DXGI_PRESENT_TEST;
	}

	HRESULT hr = IDXGISwapChain_Present(context_d3d11->swapchain, sync_interval, flags);
	context_d3d11->is_occluded = (hr == DXGI_STATUS_OCCLUDED);
	if (context_d3d11->is_occluded) {
		hr = S_OK;
	}

	context_d3d11->backbuffer_index = (context_d3d11->backbuffer_index + 1) % context_d3d11->num_backbuffers;

	return rjd_gfx_translate_hresult(hr);
}

// commands
struct rjd_result rjd_gfx_command_buffer_create(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* out)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	struct rjd_gfx_command_buffer_d3d11 cmd_buffer_d3d11 = {0};

	if (rjd_array_count(context_d3d11->free_deferred_contexts) > 0) {
		cmd_buffer_d3d11.deferred_context = rjd_array_pop(context_d3d11->free_deferred_contexts);
	} else {
		HRESULT hr = ID3D11Device1_CreateDeferredContext1(context_d3d11->device, 0, &cmd_buffer_d3d11.deferred_context);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}
	}

	rjd_slotmap_insert(context_d3d11->slotmap_command_buffers, cmd_buffer_d3d11, &out->handle);
	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_command_pass_begin(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, const struct rjd_gfx_pass_begin_desc* command)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	struct rjd_gfx_command_buffer_d3d11* cmd_buffer_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_command_buffers, cmd_buffer->handle);

	if (cmd_buffer_d3d11->render_target_view) {
		return RJD_RESULT("A render pass has already been started with this command buffer.");
	}

	// Color buffer
	if (rjd_gfx_texture_isbackbuffer(command->render_target)) {
		ID3D11Texture2D* texture_backbuffer = NULL;
		D3D11_RTV_DIMENSION view_dimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		if (context_d3d11->msaa_backbuffers) {
			struct rjd_gfx_texture* render_target = context_d3d11->msaa_backbuffers + context_d3d11->backbuffer_index;
			struct rjd_gfx_texture_d3d11* render_target_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_textures, render_target->handle);
			texture_backbuffer = render_target_d3d11->texture;

			view_dimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		} else {
			HRESULT hr = IDXGISwapChain_GetBuffer(context_d3d11->swapchain, 0, &IID_ID3D11Texture2D, (void**)&texture_backbuffer);
			if (FAILED(hr)) {
				return rjd_gfx_translate_hresult(hr);
			}
		}

		// NOTE: Even if the backbuffer is non-sRGB, we can still create a sRGB view for automatic colorspace conversion if that's what the user wanted the
		// backbuffer to be.
		D3D11_RENDER_TARGET_VIEW_DESC desc = {
			.Format = rjd_gfx_format_to_dxgi(context_d3d11->backbuffer_color_format),
			.ViewDimension = view_dimension,
		};

		HRESULT hr = ID3D11Device_CreateRenderTargetView(context_d3d11->device, (ID3D11Resource*)texture_backbuffer, &desc, &cmd_buffer_d3d11->render_target_view);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}

		// Fetching the buffer from the swapchain revs the refcount, so release it here. Getting the buffer from the MSAA render target doesn't, so no need to release.
		if (view_dimension == D3D11_RTV_DIMENSION_TEXTURE2D) {
			ID3D11Texture2D_Release(texture_backbuffer);
		}
	} else {
		RJD_ASSERTFAIL("non-backbuffer render targets are unimplemented"); // TODO
	}

	struct rjd_gfx_rgba rgba_backbuffer = rjd_gfx_format_value_to_rgba(command->clear_color);
	ID3D11DeviceContext_ClearRenderTargetView(cmd_buffer_d3d11->deferred_context, cmd_buffer_d3d11->render_target_view, rgba_backbuffer.v);

	// Depthstencil buffer
	if (rjd_gfx_texture_isbackbuffer(command->depthstencil_target)) {
		ID3D11Texture2D* texture_depthstencil = NULL;

		if (context_d3d11->msaa_depthbuffers) {
			texture_depthstencil = context_d3d11->msaa_depthbuffers[context_d3d11->backbuffer_index];
		} else {
			texture_depthstencil = context_d3d11->depthbuffers[context_d3d11->backbuffer_index];
		}

		HRESULT hr = ID3D11Device_CreateDepthStencilView(context_d3d11->device, (ID3D11Resource*)texture_depthstencil, NULL, &cmd_buffer_d3d11->depthstencil_view);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}
	} else {
		RJD_ASSERTFAIL("non-backbuffer depthbuffer targets are unimplemented"); // TODO
	}

	UINT flags = 0;
	float depth_color = 0.0f;
	uint8_t stencil_color = 0;

	if (rjd_gfx_format_isdepth(command->clear_depthstencil.type)) {
		flags |= D3D11_CLEAR_DEPTH;
		depth_color = (float)rjd_gfx_format_value_to_depth(command->clear_depthstencil);
	}

	if (rjd_gfx_format_isstencil(command->clear_depthstencil.type)) {
		flags |= D3D11_CLEAR_STENCIL;
		stencil_color = rjd_gfx_format_value_to_stencil(command->clear_depthstencil);
	}

	ID3D11DeviceContext_ClearDepthStencilView(cmd_buffer_d3d11->deferred_context, cmd_buffer_d3d11->depthstencil_view, flags, depth_color, stencil_color);

	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_command_pass_draw(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, const struct rjd_gfx_pass_draw_desc* command)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	struct rjd_gfx_command_buffer_d3d11* cmd_buffer_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_command_buffers, cmd_buffer->handle);
	const D3D11_VIEWPORT viewport =
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = (float)command->viewport->width,
		.Height = (float)command->viewport->height,
		.MinDepth = 0, 
		.MaxDepth = 1.0f,
	};
	ID3D11DeviceContext1_RSSetViewports(cmd_buffer_d3d11->deferred_context, 1, &viewport);

	// pipeline state
	struct rjd_gfx_pipeline_state_d3d11* pipeline_state_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_pipeline_states, command->pipeline_state->handle);
	ID3D11DeviceContext1_IASetInputLayout(cmd_buffer_d3d11->deferred_context, pipeline_state_d3d11->vertex_layout);
	ID3D11DeviceContext1_RSSetState(cmd_buffer_d3d11->deferred_context, pipeline_state_d3d11->rasterizer_state);

	ID3D11DeviceContext1_OMSetDepthStencilState(cmd_buffer_d3d11->deferred_context, pipeline_state_d3d11->depthstencil_state, 0);
	ID3D11DeviceContext1_OMSetRenderTargets(cmd_buffer_d3d11->deferred_context, 1, &cmd_buffer_d3d11->render_target_view, cmd_buffer_d3d11->depthstencil_view);

	struct rjd_gfx_shader_d3d11* shader_vertex_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_shaders, pipeline_state_d3d11->shader_vertex.handle);
	struct rjd_gfx_shader_d3d11* shader_pixel_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_shaders, pipeline_state_d3d11->shader_pixel.handle);

	if (shader_vertex_d3d11->vertex) {
		ID3D11DeviceContext1_VSSetShader(cmd_buffer_d3d11->deferred_context, shader_vertex_d3d11->vertex, NULL, 0);
	} else {
		return RJD_RESULT("The selected shader did not have a vertex shader.");
	}

	if (shader_pixel_d3d11->pixel) {
		ID3D11DeviceContext1_PSSetShader(cmd_buffer_d3d11->deferred_context, shader_pixel_d3d11->pixel, NULL, 0);
	} else {
		return RJD_RESULT("The selected shader did not have a pixel shader.");
	}

	// textures & sampler states
	for (size_t i = 0; i < command->count_textures; ++i) {
		RJD_ASSERT(command->textures);
		RJD_ASSERT(command->texture_indices);

		struct rjd_gfx_texture_d3d11* texture_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_textures, command->textures[i].handle);
		UINT slot = command->texture_indices[i];

		if (D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT <= slot) {
			return RJD_RESULT("Not allowed to bind to slots higher than D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1");
		}

		ID3D11DeviceContext1_PSSetShaderResources(cmd_buffer_d3d11->deferred_context, slot, 1, &texture_d3d11->resource_view);
		ID3D11DeviceContext1_PSSetSamplers(cmd_buffer_d3d11->deferred_context, slot, 1, &texture_d3d11->sampler);
	}

	// draw meshes
	for (uint32_t mesh_index = 0; mesh_index < command->count_meshes; ++mesh_index) {
		RJD_ASSERT(command->meshes);

		const struct rjd_gfx_mesh_d3d11* mesh_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_meshes, command->meshes[mesh_index].handle);

		const D3D_PRIMITIVE_TOPOLOGY primitive = rjd_gfx_primitive_to_d3d11(mesh_d3d11->primitive);
		ID3D11DeviceContext1_IASetPrimitiveTopology(cmd_buffer_d3d11->deferred_context, primitive);

		for (uint32_t buffer_index = 0; buffer_index < mesh_d3d11->count_buffers; ++buffer_index) {
			const struct rjd_gfx_mesh_buffer_d3d11* buffer_d3d11 = mesh_d3d11->buffers + buffer_index;

			if (buffer_d3d11->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_VERTEX) {
				ID3D11DeviceContext1_IASetVertexBuffers(
					cmd_buffer_d3d11->deferred_context, 
					buffer_d3d11->slot, 
					1, 
					&buffer_d3d11->buffer,
					&buffer_d3d11->stride,
					&buffer_d3d11->offset);
			}

			if (buffer_d3d11->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_VERTEX_CONSTANT ||
				buffer_d3d11->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_PIXEL_CONSTANT) {

				const struct rjd_gfx_pass_draw_buffer_offset_desc* buffer_offset_desc = NULL;
				for (size_t index_constant_desc = 0; index_constant_desc < command->count_constant_descs; ++index_constant_desc) {
					if (command->buffer_offset_descs[index_constant_desc].mesh_index == mesh_index &&
						command->buffer_offset_descs[index_constant_desc].buffer_index == buffer_index) {
						buffer_offset_desc = command->buffer_offset_descs + index_constant_desc;
					}
				}
				RJD_ASSERTMSG(buffer_offset_desc, "Unable to find a rjd_gfx_pass_draw_buffer_offset_desc for mesh %u at buffer index %u.", mesh_index, buffer_index);

				const UINT first_constant = buffer_offset_desc->offset_bytes / 16;
				const UINT num_constants = buffer_offset_desc->range_bytes / 16;

				if (buffer_d3d11->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_VERTEX_CONSTANT) {
					ID3D11DeviceContext1_VSSetConstantBuffers1(
						cmd_buffer_d3d11->deferred_context, 
						buffer_d3d11->slot, 
						1, 
						&buffer_d3d11->buffer,
						&first_constant,
						&num_constants);
				}
				if (buffer_d3d11->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_PIXEL_CONSTANT) {
					ID3D11DeviceContext1_PSSetConstantBuffers1(
							cmd_buffer_d3d11->deferred_context, 
							buffer_d3d11->slot, 
							1, 
							&buffer_d3d11->buffer,
							&first_constant,
							&num_constants);
				}
			}
		}

		ID3D11DeviceContext1_Draw(cmd_buffer_d3d11->deferred_context, mesh_d3d11->count_vertices, 0);
	}

	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_command_buffer_commit(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer)
{
	RJD_ASSERT(cmd_buffer);
	RJD_ASSERT(context);
	RJD_ASSERT(rjd_slot_isvalid(cmd_buffer->handle));

	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	struct rjd_gfx_command_buffer_d3d11* cmd_buffer_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_command_buffers, cmd_buffer->handle);

	// copy the MSAA render target to the DXGI backbuffer
	if (context_d3d11->msaa_backbuffers) {
		struct rjd_gfx_texture* source_texture = context_d3d11->msaa_backbuffers + context_d3d11->backbuffer_index;
		struct rjd_gfx_texture_d3d11* source_texture_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_textures, source_texture->handle);

		ID3D11Resource* swapchain_buffer = NULL;
		{
			HRESULT hr = IDXGISwapChain_GetBuffer(context_d3d11->swapchain, 0, &IID_ID3D11Texture2D, (void**)&swapchain_buffer);
			if (FAILED(hr)) {
				return rjd_gfx_translate_hresult(hr);
			}
		}

		ID3D11DeviceContext1_ResolveSubresource(cmd_buffer_d3d11->deferred_context,
			swapchain_buffer, 0, 
			(ID3D11Resource*)source_texture_d3d11->texture, 0, 
			rjd_gfx_format_to_dxgi(context_d3d11->backbuffer_color_format));

		ID3D11Resource_Release(swapchain_buffer);
	}

	ID3D11CommandList* commandlist = NULL;
	HRESULT hr = ID3D11DeviceContext1_FinishCommandList(cmd_buffer_d3d11->deferred_context, FALSE, &commandlist);
	if (FAILED(hr)) {
		return rjd_gfx_translate_hresult(hr);
	}

	ID3D11DeviceContext_ExecuteCommandList(context_d3d11->context, commandlist, FALSE);
	ID3D11CommandList_Release(commandlist);
	ID3D11RenderTargetView_Release(cmd_buffer_d3d11->render_target_view);
	ID3D11DepthStencilView_Release(cmd_buffer_d3d11->depthstencil_view);

	rjd_array_push(context_d3d11->free_deferred_contexts, cmd_buffer_d3d11->deferred_context);

	cmd_buffer_d3d11->deferred_context = NULL;
	cmd_buffer_d3d11->render_target_view = NULL;
	cmd_buffer_d3d11->depthstencil_view = NULL;

	rjd_slotmap_erase(context_d3d11->slotmap_command_buffers, cmd_buffer->handle);
	rjd_slot_invalidate(&cmd_buffer->handle);

	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_texture_create(struct rjd_gfx_context* context, struct rjd_gfx_texture* out, struct rjd_gfx_texture_desc desc)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	DXGI_SAMPLE_DESC sample_desc = {0};
	struct rjd_result result = rjd_gfx_backbuffer_get_msaa_quality_d3d11(context_d3d11, desc.msaa_samples, &sample_desc);
	if (!rjd_result_isok(result)) {
		return result;
	}

	UINT bind_flags = 0;
	bind_flags |= D3D11_BIND_SHADER_RESOURCE;
	bind_flags |= (desc.usage == RJD_GFX_TEXTURE_USAGE_RENDERTARGET) ? D3D11_BIND_RENDER_TARGET : 0;

	const D3D11_TEXTURE2D_DESC desc_d3d11 = {
		.Width = desc.pixels_width,
		.Height = desc.pixels_height,
		.MipLevels = 1, // TODO mipmap support
		.ArraySize = 1,
		.Format = rjd_gfx_format_to_dxgi(desc.format),
		.SampleDesc = sample_desc,
		.Usage = rjd_gfx_texture_access_to_gpu_access(desc.access),
		.BindFlags = bind_flags,
		.CPUAccessFlags = rjd_gfx_texture_access_to_cpu_access(desc.access),
		.MiscFlags = 0,
	};

	const D3D11_SUBRESOURCE_DATA subresource = {
		.pSysMem = desc.data,
		.SysMemPitch = desc.pixels_width * rjd_gfx_format_bytesize(desc.format),
		.SysMemSlicePitch = 0, // only for 3D textures
	};

	ID3D11Texture2D* texture = NULL;
	{
		const D3D11_SUBRESOURCE_DATA* initial_data = desc.data ? &subresource : NULL;

		HRESULT hr = ID3D11Device_CreateTexture2D(context_d3d11->device, &desc_d3d11, initial_data, &texture);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}
	}

	ID3D11ShaderResourceView* resource_view = NULL;
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc_resource_view = 
		{
			.Format = desc_d3d11.Format,
		};

		if (sample_desc.Count > 1) {
			desc_resource_view.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		} else {
			desc_resource_view.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc_resource_view.Texture2D.MostDetailedMip = 0;
			desc_resource_view.Texture2D.MipLevels = desc_d3d11.MipLevels;
		}

		HRESULT hr = ID3D11Device_CreateShaderResourceView(context_d3d11->device, (ID3D11Resource*)texture, &desc_resource_view, &resource_view);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}
	}

	ID3D11SamplerState* sampler = NULL;
	{
		// TODO make this configurable
		D3D11_SAMPLER_DESC desc_sampler = {
			.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
			.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
			.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
			.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
			.MipLODBias = 0.0f,
			.MaxAnisotropy = 0,
			.ComparisonFunc = D3D11_COMPARISON_ALWAYS,
			.MinLOD = 0,
			.MaxLOD = D3D11_FLOAT32_MAX,
		};

		HRESULT hr = ID3D11Device_CreateSamplerState(context_d3d11->device, &desc_sampler, &sampler);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}
	}

	struct rjd_gfx_texture_d3d11 texture_d3d11 = {
		.texture = texture,
		.resource_view = resource_view,
		.sampler = sampler,
		.debug_name = rjd_strpool_add(&context_d3d11->debug_names, desc.debug_label),
	};

	rjd_slotmap_insert(context_d3d11->slotmap_textures, texture_d3d11, &out->handle);

	return RJD_RESULT_OK();
}

void rjd_gfx_texture_destroy(struct rjd_gfx_context* context, struct rjd_gfx_texture* texture)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	rjd_gfx_texture_destroy_d3d11(context_d3d11, texture->handle);
}

struct rjd_result rjd_gfx_shader_create(struct rjd_gfx_context* context, struct rjd_gfx_shader* out, struct rjd_gfx_shader_desc desc)
{
	RJD_ASSERT(context);
	RJD_ASSERT(desc.data);
	RJD_ASSERT(desc.count_data > 0);
	RJD_ASSERT(desc.function_name);

	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	const char* target = NULL;
	switch (desc.type)
	{
		case RJD_GFX_SHADER_TYPE_VERTEX:	target = "vs_5_0"; break;
		case RJD_GFX_SHADER_TYPE_PIXEL:		target = "ps_5_0"; break;
	}

	UINT flags = 0;
	flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
	flags |= D3DCOMPILE_DEBUG; // TODO make this optional

	ID3DBlob* bytecode = NULL;
	ID3DBlob* errors = NULL;

	HRESULT hr = D3DCompile(
		desc.data,
		desc.count_data,
		desc.source_name,
		NULL,
		NULL,
		desc.function_name,
		target,
		flags,
		0,
		&bytecode,
		&errors);
	if (FAILED(hr)) {
		void* errors_data = ID3D10Blob_GetBufferPointer(errors);
		size_t errors_size = ID3D10Blob_GetBufferSize(errors);

		char* error_string = rjd_mem_alloc_array(char, errors_size + 1, context_d3d11->allocator);
		memcpy(error_string, errors_data, errors_size);
		error_string[errors_size] = 0;
		RJD_LOG("Shader compile errors: %s\n", error_string);
		return RJD_RESULT("Shaders failed to compile.");
	}

	ID3D11VertexShader* vertex = NULL;
	ID3D11PixelShader* pixel = NULL;

	const void* bytecode_data = ID3D10Blob_GetBufferPointer(bytecode);
	const size_t bytecode_size = ID3D10Blob_GetBufferSize(bytecode);

	if (desc.type == RJD_GFX_SHADER_TYPE_VERTEX) {
		hr = ID3D11Device_CreateVertexShader(context_d3d11->device, bytecode_data, bytecode_size, NULL, &vertex);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}
	}

	if (desc.type == RJD_GFX_SHADER_TYPE_PIXEL) {
		hr = ID3D11Device_CreatePixelShader(context_d3d11->device, bytecode_data, bytecode_size, NULL, &pixel);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}
	}

	struct rjd_gfx_shader_d3d11 shader_d3d11 = {
		.bytecode = bytecode,
		.vertex = vertex,
		.pixel = pixel,
		.debug_name = rjd_strpool_add(&context_d3d11->debug_names, desc.function_name),
		.debug_source = rjd_strpool_add(&context_d3d11->debug_names, desc.source_name),
	};

	rjd_slotmap_insert(context_d3d11->slotmap_shaders, shader_d3d11, &out->handle);

	return RJD_RESULT_OK();
}

void rjd_gfx_shader_destroy(struct rjd_gfx_context* context, struct rjd_gfx_shader* shader)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	rjd_gfx_shader_destroy_d3d11(context_d3d11, shader->handle);
}

struct rjd_result rjd_gfx_pipeline_state_create(struct rjd_gfx_context* context, struct rjd_gfx_pipeline_state* out, struct rjd_gfx_pipeline_state_desc desc)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	ID3D11InputLayout* vertex_layout = NULL;

	{
		uint32_t semantic_index[RJD_GFX_VERTEX_SEMANTIC_COUNT] = {0};

		D3D11_INPUT_ELEMENT_DESC* element_descs = rjd_mem_alloc_array(D3D11_INPUT_ELEMENT_DESC, desc.count_vertex_attributes, context_d3d11->allocator);
		for (size_t i = 0; i < desc.count_vertex_attributes; ++i) {
			DXGI_FORMAT format = 0;
			switch (desc.vertex_attributes[i].type) {
				case RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT1: format = DXGI_FORMAT_R32_FLOAT; break;
				case RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT2: format = DXGI_FORMAT_R32G32_FLOAT; break;
				case RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT3: format = DXGI_FORMAT_R32G32B32_FLOAT; break;
				case RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT4: format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				case RJD_GFX_VERTEX_FORMAT_TYPE_COUNT: RJD_ASSERT("Invalid vertex format type RJD_GFX_VERTEX_FORMAT_TYPE_COUNT"); break;
			}
		
			const enum rjd_gfx_vertex_semantic semantic = desc.vertex_attributes[i].semantic;

			if (semantic == RJD_GFX_VERTEX_SEMANTIC_POSITION) {
				RJD_ASSERTMSG(semantic_index[semantic] == 0, "You can only have 1 POSITION semantic in an input layout.");
			}

			element_descs[i].SemanticName = rjd_gfx_semantic_to_name(semantic);
			element_descs[i].SemanticIndex = semantic_index[semantic];
			element_descs[i].Format = format;
			element_descs[i].InputSlot = desc.vertex_attributes[i].shader_slot_d3d11;
			element_descs[i].AlignedByteOffset = desc.vertex_attributes[i].offset;
			element_descs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // TODO support D3D11_INPUT_PER_INSTANCE_DATA
			element_descs[i].InstanceDataStepRate = 0;

			++semantic_index[semantic];
		}

		const struct rjd_gfx_shader_d3d11* shader_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_shaders, desc.shader_vertex.handle);
		const void* bytecode = ID3D10Blob_GetBufferPointer(shader_d3d11->bytecode);
		const size_t bytecode_size = ID3D10Blob_GetBufferSize(shader_d3d11->bytecode);

		HRESULT result = ID3D11Device_CreateInputLayout(context_d3d11->device, element_descs, desc.count_vertex_attributes, bytecode, bytecode_size, &vertex_layout);
		if (FAILED(result)) {
			return rjd_gfx_translate_hresult(result);
		}

		rjd_mem_free(element_descs);
	}

	ID3D11RasterizerState* rasterizer_state = NULL;
	{
		D3D11_RASTERIZER_DESC desc_rasterizer = 
		{
			.FillMode = D3D11_FILL_SOLID,
			.CullMode = rjd_gfx_cull_to_d3d(desc.cull_mode),
			.FrontCounterClockwise = desc.winding_order == RJD_GFX_WINDING_ORDER_COUNTERCLOCKWISE,
			.DepthBias = 0,
			.DepthBiasClamp = 1.0f,
			.SlopeScaledDepthBias = 1.0f,
			.DepthClipEnable = TRUE,
			.ScissorEnable = FALSE, // TODO support stencils
			.MultisampleEnable = TRUE,
			.AntialiasedLineEnable = FALSE,
		};

		HRESULT result = ID3D11Device_CreateRasterizerState(context_d3d11->device, &desc_rasterizer, &rasterizer_state);
		if (FAILED(result)) {
			return rjd_gfx_translate_hresult(result);
		}
	}

	ID3D11DepthStencilState* depthstencil_state = NULL;
	{
		D3D11_DEPTH_STENCIL_DESC desc_depthstencil = {
			.DepthEnable = TRUE,
			.DepthWriteMask = desc.depth_write_enabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO,
			.DepthFunc = rjd_gfx_depth_compare_to_d3d11(desc.depth_compare),

			.StencilEnable = FALSE,
			// .StencilReadMask = 0xFF,
			// .StencilWriteMask = 0xFF,

			// .FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			// .FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR,
			// .FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			// .FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS,

			// .BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			// .BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR,
			// .BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			// .BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS,
		};

		HRESULT hr = ID3D11Device_CreateDepthStencilState(context_d3d11->device, &desc_depthstencil, &depthstencil_state);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}
	}

	struct rjd_gfx_pipeline_state_d3d11 state_d3d11 = 
	{
		.debug_name = rjd_strpool_add(&context_d3d11->debug_names, desc.debug_name),
		.vertex_layout = vertex_layout,
		.rasterizer_state = rasterizer_state,
		.shader_vertex = desc.shader_vertex,
		.shader_pixel = desc.shader_pixel,
		.render_target = desc.render_target,
		.depthstencil_target = desc.depthstencil_target,
		.depthstencil_state = depthstencil_state,
	};
	
	rjd_slotmap_insert(context_d3d11->slotmap_pipeline_states, state_d3d11, &out->handle);

	return RJD_RESULT_OK();
}

void rjd_gfx_pipeline_state_destroy(struct rjd_gfx_context* context, struct rjd_gfx_pipeline_state* pipeline_state)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	rjd_gfx_pipeline_state_destroy_d3d11(context_d3d11, pipeline_state->handle);
}

struct rjd_result rjd_gfx_mesh_create_vertexed(struct rjd_gfx_context* context, struct rjd_gfx_mesh* out, struct rjd_gfx_mesh_vertexed_desc desc)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	struct rjd_gfx_mesh_buffer_d3d11* buffers = rjd_mem_alloc_array(struct rjd_gfx_mesh_buffer_d3d11, desc.count_buffers, context_d3d11->allocator);

	for (size_t i = 0; i < desc.count_buffers; ++i) {
		const struct rjd_gfx_mesh_buffer_desc* desc_buffer = desc.buffers + i;

		UINT buffer_size = 0;
		UINT flags_bind = 0;
		uint32_t stride = 0;
		const void* data = NULL;

		if (desc_buffer->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_VERTEX) {
			if (desc_buffer->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_VERTEX_CONSTANT ||
				desc_buffer->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_PIXEL_CONSTANT) {
				return RJD_RESULT("Vertex buffers cannot also be constant buffers.");
			}

				buffer_size = desc_buffer->common.vertex.length;
				stride = desc_buffer->common.vertex.stride;
				data = desc_buffer->common.vertex.data;
				flags_bind = D3D11_BIND_VERTEX_BUFFER;

				if (desc_buffer->shader_slot_d3d11 >= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1) {
					return RJD_RESULT("Vertex buffer index must be < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT");
				}
				if (stride == 0) {
					return RJD_RESULT("Vertex buffers must have a stride larger than 0.");
				}
		}

		if (desc_buffer->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_VERTEX_CONSTANT || 
			desc_buffer->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_PIXEL_CONSTANT) {
				buffer_size = desc_buffer->common.constant.capacity;
				flags_bind = D3D11_BIND_CONSTANT_BUFFER;

				if (buffer_size % 16 != 0) {
					return RJD_RESULT("Uniform buffer size must be a multiple of 16.");
				}
				if (D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT * 4 * sizeof(float) < buffer_size) {
					return RJD_RESULT("Uniform buffer exceeded max allowable size");
				}
				if (desc_buffer->shader_slot_d3d11 >= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1) {
					return RJD_RESULT("Constant buffer index must be < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT");
				}	

				// Despite the documentation claiming otherwise, D3D11 requires valid data for constant 
				// buffers. We'll just give it an array of zeroes to work with for now.
				void* temp_data = rjd_mem_alloc_stack_array_noclear(uint8_t, buffer_size); // TODO verify this doesn't get destroyed once we leave switch scope?
				memset(temp_data, 0, buffer_size);
				for (size_t j = 0; j < buffer_size; j += sizeof(float)) {
					float* f = (void*)((char*)temp_data + j);
					*f = 255.0f;
				}

				data = temp_data;
		}

		D3D11_BUFFER_DESC desc_buffer_d3d11 = {
			.ByteWidth = buffer_size,
			.Usage = D3D11_USAGE_DYNAMIC, // TODO expose options for immutable buffers
			.BindFlags = flags_bind,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0,
			.StructureByteStride = 0,
		};

		D3D11_SUBRESOURCE_DATA subresource = {
			.pSysMem = data,
			.SysMemPitch = 0,
			.SysMemSlicePitch = 0,
		};

		ID3D11Buffer* buffer_d3d11 = NULL;
		HRESULT hr = ID3D11Device_CreateBuffer(context_d3d11->device, &desc_buffer_d3d11, &subresource, &buffer_d3d11);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}

		struct rjd_gfx_mesh_buffer_d3d11 buffer = {
			.buffer = buffer_d3d11,
			.stride = stride,
			.offset = 0,
			.slot = desc_buffer->shader_slot_d3d11,
			.usage_flags = desc_buffer->usage_flags,
		};

		buffers[i] = buffer;
	}

	struct rjd_gfx_mesh_d3d11 mesh_d3d11 = {
		.buffers = buffers,
		.count_buffers = desc.count_buffers,
		.count_vertices = desc.count_vertices,
		.primitive = desc.primitive,
	};

	rjd_slotmap_insert(context_d3d11->slotmap_meshes, mesh_d3d11, &out->handle);

	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_mesh_modify(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, struct rjd_gfx_mesh* mesh, uint32_t buffer_index, uint32_t offset, const void* data, uint32_t length)
{
	RJD_ASSERT(context);
	RJD_ASSERT(cmd_buffer);
	RJD_ASSERT(mesh);
	RJD_ASSERT(data || length == 0);

	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	struct rjd_gfx_command_buffer_d3d11* command_buffer_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_command_buffers, cmd_buffer->handle);
	struct rjd_gfx_mesh_d3d11* mesh_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_meshes, mesh->handle);

	RJD_ASSERTMSG(buffer_index < mesh_d3d11->count_buffers, "Invalid index into mesh buffer array");

	struct rjd_gfx_mesh_buffer_d3d11* buffer = mesh_d3d11->buffers + buffer_index;

	ID3D11Resource* resource = (ID3D11Resource*)buffer->buffer;
	const UINT subresource = 0;
	D3D11_MAPPED_SUBRESOURCE gpu_resource = {0};

	HRESULT hr = ID3D11DeviceContext_Map(command_buffer_d3d11->deferred_context, resource, subresource, D3D11_MAP_WRITE_DISCARD, 0, &gpu_resource);
	if (FAILED(hr)) {
		return rjd_gfx_translate_hresult(hr);
	}

	memcpy((char*)gpu_resource.pData + offset, data, length);

	ID3D11DeviceContext_Unmap(command_buffer_d3d11->deferred_context, resource, subresource);
	return RJD_RESULT_OK();
}

void rjd_gfx_mesh_destroy(struct rjd_gfx_context* context, struct rjd_gfx_mesh* mesh)
{
	RJD_ASSERT(context);
	RJD_ASSERT(mesh);

	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	rjd_gfx_mesh_destroy_d3d11(context_d3d11, mesh->handle);
}

////////////////////////////////////////////////////////////////////////////////
// local implementation

static struct rjd_result rjd_gfx_translate_hresult(HRESULT hr)
{
	if (SUCCEEDED(hr)) {
		return RJD_RESULT_OK();
	}

	switch (hr) 
	{
	case E_INVALIDARG:
		return RJD_RESULT("E_INVALIDARG");

	default:
		return RJD_RESULT("Failed. TODO more info.");
	}
}

static DXGI_FORMAT rjd_gfx_format_to_dxgi(enum rjd_gfx_format format)
{
	switch (format)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA: return DXGI_FORMAT_R8G8B8A8_UINT;
		case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM: return DXGI_FORMAT_B8G8R8A8_UNORM;
		case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32: return DXGI_FORMAT_D32_FLOAT;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case RJD_GFX_FORMAT_COUNT: break;
	}

	RJD_ASSERTFAIL("Unhandled case.");
	return DXGI_FORMAT_UNKNOWN;
}

static DXGI_FORMAT rjd_gfx_format_to_dxgi_strip_srgb(enum rjd_gfx_format format)
{
	enum rjd_gfx_format format_no_srgb = format;

	switch (format_no_srgb)
	{
		case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB: format_no_srgb = RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM; break;
		default: break;
	}

	return rjd_gfx_format_to_dxgi(format_no_srgb);
}

static struct rjd_gfx_rgba rjd_gfx_format_value_to_rgba(struct rjd_gfx_format_value value)
{
	struct rjd_gfx_rgba out = {0};
	switch (value.type)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA:
			out.v[0] = value.color_u8_rgba[0] / 255.0f;
			out.v[1] = value.color_u8_rgba[1] / 255.0f;
			out.v[2] = value.color_u8_rgba[2] / 255.0f;
			out.v[3] = value.color_u8_rgba[3] / 255.0f;
			break;
		case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM:
		case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB:
			RJD_ASSERT("TODO");
			break;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32:
		case RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8:
			RJD_ASSERT("This is intended to be used by colors, not depthstencil values.");
			break;
		case RJD_GFX_FORMAT_COUNT:
			RJD_ASSERTFAIL("Bad type.");
			break;
	}

	return out;
}

D3D_PRIMITIVE_TOPOLOGY rjd_gfx_primitive_to_d3d11(enum rjd_gfx_primitive_type primitive)
{
	switch (primitive)
	{
		case RJD_GFX_PRIMITIVE_TYPE_TRIANGLES: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	RJD_ASSERTFAIL("Unhandled primitive type %d", primitive);
	return 0;
}

static D3D11_CULL_MODE rjd_gfx_cull_to_d3d(enum rjd_gfx_cull cull_mode)
{
	switch (cull_mode)
	{
		case RJD_GFX_CULL_NONE: return D3D11_CULL_NONE;
		case RJD_GFX_CULL_BACK: return D3D11_CULL_BACK;
		case RJD_GFX_CULL_FRONT: return D3D11_CULL_FRONT;
	}

	RJD_ASSERTFAIL("Unhandled case %d", cull_mode);
	return D3D11_CULL_NONE;
}

static D3D11_COMPARISON_FUNC rjd_gfx_depth_compare_to_d3d11(enum rjd_gfx_depth_compare compare)
{
	switch (compare)
	{
		case RJD_GFX_DEPTH_COMPARE_ALWAYS_SUCCEED: return D3D11_COMPARISON_ALWAYS;
		case RJD_GFX_DEPTH_COMPARE_ALWAYS_FAIL: return D3D11_COMPARISON_NEVER;
		case RJD_GFX_DEPTH_COMPARE_LESS: return D3D11_COMPARISON_LESS;
		case RJD_GFX_DEPTH_COMPARE_LESSEQUAL: return D3D11_COMPARISON_LESS_EQUAL;
		case RJD_GFX_DEPTH_COMPARE_GREATER: return D3D11_COMPARISON_GREATER;
		case RJD_GFX_DEPTH_COMPARE_GREATEREQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
		case RJD_GFX_DEPTH_COMPARE_EQUAL: return D3D11_COMPARISON_EQUAL;
		case RJD_GFX_DEPTH_COMPARE_NOTEQUAL: return D3D11_COMPARISON_NOT_EQUAL;
	}

	RJD_ASSERTFAIL("Unhandled case %d", compare);
	return D3D11_COMPARISON_ALWAYS;
}

static D3D11_USAGE rjd_gfx_texture_access_to_gpu_access(enum rjd_gfx_texture_access access)
{
	switch (access)
	{
		case RJD_GFX_TEXTURE_ACCESS_CPU_WRITE_GPU_READWRITE: return D3D11_USAGE_DEFAULT;
		case RJD_GFX_TEXTURE_ACCESS_CPU_NONE_GPU_READWRITE: return D3D11_USAGE_DEFAULT;
		case RJD_GFX_TEXTURE_ACCESS_COUNT: break;
	}

	RJD_ASSERTFAIL("Unhandled case %d", access);
	return D3D11_USAGE_DEFAULT;
}

static D3D11_CPU_ACCESS_FLAG rjd_gfx_texture_access_to_cpu_access(enum rjd_gfx_texture_access access)
{
	switch (access)
	{
		case RJD_GFX_TEXTURE_ACCESS_CPU_WRITE_GPU_READWRITE: return D3D11_CPU_ACCESS_WRITE;
		case RJD_GFX_TEXTURE_ACCESS_CPU_NONE_GPU_READWRITE: return 0;
		case RJD_GFX_TEXTURE_ACCESS_COUNT: break;
	}

	RJD_ASSERTFAIL("Unhandled case %d", access);
	return 0;
}

static const char* rjd_gfx_semantic_to_name(enum rjd_gfx_vertex_semantic semantic)
{
	switch (semantic)
	{
	case RJD_GFX_VERTEX_SEMANTIC_POSITION: return "SV_Position";
	case RJD_GFX_VERTEX_SEMANTIC_COLOR:	return "COLOR";
	case RJD_GFX_VERTEX_SEMANTIC_NORMAL: return "NORMAL";
	case RJD_GFX_VERTEX_SEMANTIC_TEXCOORD: return "TEXCOORD";
	case RJD_GFX_VERTEX_SEMANTIC_BINORMAL: return "BINORMAL";
	case RJD_GFX_VERTEX_SEMANTIC_TANGENT: return "TANGENT";
	case RJD_GFX_VERTEX_SEMANTIC_BLENDINDEX: return "BLENDINDICES";
	case RJD_GFX_VERTEX_SEMANTIC_BLENDWEIGHT: return "BLENDWEIGHT";
	case RJD_GFX_VERTEX_SEMANTIC_COUNT: RJD_ASSERTFAIL("RJD_GFX_VERTEX_SEMANTIC_COUNT is an invalid semantic type"); return NULL;
	}

	RJD_ASSERTFAIL("Unhandled case %d", semantic);
	return NULL;
}

static bool rjd_gfx_texture_isbackbuffer(struct rjd_gfx_texture texture)
{
	return	texture.handle.salt == RJD_GFX_TEXTURE_BACKBUFFER.handle.salt &&
			texture.handle.index == RJD_GFX_TEXTURE_BACKBUFFER.handle.index;
}

static struct rjd_result rjd_gfx_backbuffer_get_msaa_quality_d3d11(const struct rjd_gfx_context_d3d11* context, uint32_t sample_count, DXGI_SAMPLE_DESC* out)
{
	RJD_ASSERTMSG(sample_count != 0, "You must supply a sample_count greater than 0 that is a power of 2.");
	// RJD_ASSERTMSG(rjd_math_next_pow2(sample_cont) == sample_count, "sample_count (%u) must be a power of 2.", sample_count); // TODO

	UINT quality = 0;

	if (sample_count > 1) {
		DXGI_FORMAT format = rjd_gfx_format_to_dxgi(context->backbuffer_color_format);

		HRESULT hr = ID3D11Device_CheckMultisampleQualityLevels(context->device, format, sample_count, &quality);
		if (FAILED(hr)) {
			if (hr == 0) {
				return RJD_RESULT("The MSAA level was unsupported. Try a lower level. It's also possible your backbuffer format doesn't support this level of MSAA.");
			} else {
				return RJD_RESULT("Failed to get multisample quality levels.");
			}
		}

		if (quality == 0) {
			return RJD_RESULT("The MSAA level was unsupported.");
		}

		// CheckMultisampleQualityLevels() returns the "count" of quality levels supported, so the final desc Quality is minus one
		quality = quality - 1;
	}

	out->Count = sample_count;
	out->Quality = quality;

	return RJD_RESULT_OK();
}

static inline void rjd_gfx_texture_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot)
{
	struct rjd_gfx_texture_d3d11* texture = rjd_slotmap_get(context->slotmap_textures, slot);

	ID3D11Texture2D_Release(texture->texture);
	ID3D11ShaderResourceView_Release(texture->resource_view);
	ID3D11SamplerState_Release(texture->sampler);
	rjd_strref_release(texture->debug_name);

	rjd_slotmap_erase(context->slotmap_textures, slot);
}

static inline void rjd_gfx_shader_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot)
{
	struct rjd_gfx_shader_d3d11* shader = rjd_slotmap_get(context->slotmap_shaders, slot);

	ID3D10Blob_Release(shader->bytecode);
	if (shader->vertex) {
		ID3D11VertexShader_Release(shader->vertex);
	}
	if (shader->pixel) {
		ID3D11PixelShader_Release(shader->pixel);
	}
	rjd_strref_release(shader->debug_name);
	rjd_strref_release(shader->debug_source);

	rjd_slotmap_erase(context->slotmap_shaders, slot);
}

static inline void rjd_gfx_pipeline_state_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot)
{
	struct rjd_gfx_pipeline_state_d3d11* pipeline_state = rjd_slotmap_get(context->slotmap_pipeline_states, slot);

	ID3D11InputLayout_Release(pipeline_state->vertex_layout);
	ID3D11RasterizerState_Release(pipeline_state->rasterizer_state);
	ID3D11DepthStencilState_Release(pipeline_state->depthstencil_state);
	rjd_strref_release(pipeline_state->debug_name);

	rjd_slotmap_erase(context->slotmap_pipeline_states, slot);
}

static inline void rjd_gfx_mesh_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot)
{
	struct rjd_gfx_mesh_d3d11* mesh = rjd_slotmap_get(context->slotmap_meshes, slot);

	for (size_t i = 0; i < mesh->count_buffers; ++i) {
		ID3D11Buffer_Release(mesh->buffers[i].buffer);
	}
	rjd_mem_free(mesh->buffers);

	rjd_slotmap_erase(context->slotmap_meshes, slot);
}

static inline void rjd_gfx_command_buffer_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot)
{
	struct rjd_gfx_command_buffer_d3d11* cmd_buffer = rjd_slotmap_get(context->slotmap_command_buffers, slot);
	if (cmd_buffer->render_target_view) {
		ID3D11RenderTargetView_Release(cmd_buffer->render_target_view);
	}
	if (cmd_buffer->depthstencil_view) {
		ID3D11RenderTargetView_Release(cmd_buffer->depthstencil_view);
	}
	if (cmd_buffer->deferred_context) {
		ID3D11DeviceContext_Release(cmd_buffer->deferred_context);
	}

	rjd_slotmap_erase(context->slotmap_command_buffers, slot);
}
