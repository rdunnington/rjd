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

#define CINTERFACE
#define COBJMACROS
#include <d3d11.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#undef CINTERFACE
#undef COBJMACROS

#if 1

////////////////////////////////////////////////////////////////////////////////
// Local helpers

#if RJD_COMPILER_GCC
	// The GCC headers have all the correct declarations for these GUIDs, but libdxguid.a are missing the definitions
	const GUID IID_IDXGIFactory4 = { 0x1bc6ea02, 0xef36, 0x464f, { 0xbf,0x0c,0x21,0xca,0x39,0xe5,0x16,0x8a } };
#endif

struct rjd_gfx_context_d3d11
{
	IDXGIFactory4* factory;
	IDXGIAdapter1* adapter;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain1* swapchain;

	struct rjd_gfx_texture_d3d11* slotmap_textures;
	struct rjd_gfx_shader_d3d11* slotmap_shaders;
	struct rjd_gfx_pipeline_state_d3d11* slotmap_pipeline_states;
	struct rjd_gfx_mesh_d3d11* slotmap_meshes;
	struct rjd_gfx_command_buffer_d3d11* slotmap_command_buffers;

	struct rjd_mem_allocator* allocator;
	HWND hwnd;
	bool is_occluded;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_gfx_context_d3d11) <= sizeof(struct rjd_gfx_context));

struct rjd_gfx_texture_d3d11
{
	int a;
};

struct rjd_gfx_shader_d3d11
{
	int a;
};

struct rjd_gfx_pipeline_state_d3d11
{
	const char* debug_name;
	struct rjd_gfx_shader shader;
	struct rjd_gfx_texture render_target;
	struct rjd_gfx_texture depthstencil_target;
	struct rjd_gfx_vertex_format_attribute* vertex_attributes; // rjd_array
	enum rjd_gfx_depth_compare depth_compare;
};

struct rjd_gfx_mesh_d3d11
{
	int a;
};

struct rjd_gfx_command_buffer_d3d11
{
	ID3D11DeviceContext* deferred_context;
	ID3D11RenderTargetView* render_target;
};

struct rjd_gfx_rgba
{
	float v[4];
};

// Local helpers
static struct rjd_result rjd_gfx_translate_hresult(HRESULT hr);

static DXGI_FORMAT rjd_gfx_format_to_dxgi(enum rjd_gfx_format format);
static struct rjd_gfx_rgba rjd_gfx_format_value_to_rgba(struct rjd_gfx_format_value value);

static bool rjd_gfx_texture_isbackbuffer(struct rjd_gfx_texture texture);

static inline void rjd_gfx_texture_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot);
static inline void rjd_gfx_shader_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot);
static inline void rjd_gfx_pipeline_state_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot);
static inline void rjd_gfx_mesh_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot);
static inline void rjd_gfx_command_buffer_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot);

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

	const UINT flags = 
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | // enable compatibility with Direct2D.
		D3D11_CREATE_DEVICE_DEBUG | // TODO make this optional
		0;

	const D3D_FEATURE_LEVEL feature_levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	IDXGIAdapter1* adapter = NULL;
	struct rjd_result result_adapter = RJD_RESULT("No hardware adapter found. Does this machine have a GPU?");
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != IDXGIFactory1_EnumAdapters1(factory, i, &adapter); ++i) {
		DXGI_ADAPTER_DESC1 desc_adapter;
		IDXGIAdapter1_GetDesc1(adapter, &desc_adapter);
		if (desc_adapter.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			adapter = NULL;
			continue;
		}

		// desc_adapter.DedicatedVideoMemory
		// desc_adapter.Description // wchar_t* name
	}
	if (!rjd_result_isok(result_adapter)) {
		return result_adapter;
	}

	ID3D11Device* device = NULL;
	ID3D11DeviceContext* context = NULL;
	const HRESULT hr_device = D3D11CreateDevice(
		(IDXGIAdapter*)adapter, // TODO verify this cast is legit
		D3D_DRIVER_TYPE_HARDWARE,
		NULL, // not using a software rasterizer
		flags,
		feature_levels,
		rjd_countof(feature_levels),
		D3D11_SDK_VERSION,
		&device,
		NULL,
		&context);

	if (FAILED(hr_device)) {
		if (hr_device == DXGI_ERROR_SDK_COMPONENT_MISSING) {
			if (flags & D3D11_CREATE_DEVICE_DEBUG) {
				return RJD_RESULT("Correct debug layer version not available on this machine. "
									"Make sure the DirectX SDK is up to date.");
			} else {
				return RJD_RESULT("A SDK component was not available.");
			}
		} else {
			return rjd_gfx_translate_hresult(hr_device); 
		}
	}

	const DXGI_FORMAT backbuffer_format = rjd_gfx_format_to_dxgi(desc.backbuffer_color_format);
	{
		UINT support;
		HRESULT hr = ID3D11Device_CheckFormatSupport(device, backbuffer_format, &support);
		if (hr == E_FAIL) {
			return RJD_RESULT("Device does not support specified backbuffer format.");
		}
	}

	IDXGISwapChain1* swapchain = NULL;
	{
		DXGI_RATIONAL refresh_rate = {
			.Numerator = 0,
			.Denominator = 1,
		};

		DXGI_SAMPLE_DESC desc_msaa = {
			.Count = 1,
			.Quality = 0,
		};

		if (desc.optional_desired_msaa_samples) {
			for (uint32_t i = 0; i < desc.desired_msaa_samples_count; ++i) {
				UINT quality = 0;
				UINT count = desc.optional_desired_msaa_samples[i];
				HRESULT hr = ID3D11Device_CheckMultisampleQualityLevels(device, backbuffer_format, count, &quality);
				if (SUCCEEDED(hr)) {
					desc_msaa.Count = count;
					desc_msaa.Quality = quality;
				} else if (hr == 0) {
					// unsupported
				} else {
					return RJD_RESULT("Failed to get multisample quality levels.");
				}
			}
		}

		DXGI_SWAP_CHAIN_DESC1 desc_swapchain = {
			.Width = 0, // auto-detect
			.Height = 0, // auto-detect
			.Format = backbuffer_format,
			.Stereo = false,
			.SampleDesc = desc_msaa,
			.BufferUsage = DXGI_USAGE_BACK_BUFFER,
			.BufferCount = 3, // TODO make this configurable. Right now 3 matches the metal defaults.
			.Scaling = DXGI_SCALING_NONE, // Don't stretch the backbuffer to fit the window size. We should handle that.
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD, // flip model has better perf and discard since we don't care to keep it around
			.AlphaMode = DXGI_ALPHA_MODE_STRAIGHT,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING, // needed to turn off vsync
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

    memset(out, 0, sizeof(*out));
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)out;

	context_d3d11->slotmap_textures			= rjd_slotmap_alloc(struct rjd_gfx_texture_d3d11, 64, desc.allocator);
	context_d3d11->slotmap_shaders			= rjd_slotmap_alloc(struct rjd_gfx_shader_d3d11, 64, desc.allocator);
	context_d3d11->slotmap_pipeline_states	= rjd_slotmap_alloc(struct rjd_gfx_pipeline_state_d3d11, 64, desc.allocator);
	context_d3d11->slotmap_meshes			= rjd_slotmap_alloc(struct rjd_gfx_mesh_d3d11, 64, desc.allocator);
	context_d3d11->slotmap_command_buffers	= rjd_slotmap_alloc(struct rjd_gfx_command_buffer_d3d11, 16, desc.allocator);

	context_d3d11->factory = factory;
	context_d3d11->adapter = adapter;
	context_d3d11->device = device;
	context_d3d11->context = context;
	context_d3d11->swapchain = swapchain;

	context_d3d11->allocator = desc.allocator;
	context_d3d11->hwnd = desc.win32.hwnd;
	context_d3d11->is_occluded = false;

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

	for (struct rjd_slot s = rjd_slotmap_next(textures, NULL); !rjd_slot_isvalid(s); s = rjd_slotmap_next(textures, &s)) {
		rjd_gfx_texture_destroy_d3d11(context_d3d11, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(shaders, NULL); !rjd_slot_isvalid(s); s = rjd_slotmap_next(shaders, &s)) {
		rjd_gfx_shader_destroy_d3d11(context_d3d11, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(states, NULL); !rjd_slot_isvalid(s); s = rjd_slotmap_next(states, &s)) {
		rjd_gfx_pipeline_state_destroy_d3d11(context_d3d11, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(meshes, NULL); !rjd_slot_isvalid(s); s = rjd_slotmap_next(meshes, &s)) {
		rjd_gfx_mesh_destroy_d3d11(context_d3d11, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(commands, NULL); !rjd_slot_isvalid(s); s = rjd_slotmap_next(commands, &s)) {
		rjd_gfx_command_buffer_destroy_d3d11(context_d3d11, s);
	}

	rjd_slotmap_free(textures);
	rjd_slotmap_free(shaders);
	rjd_slotmap_free(states);
	rjd_slotmap_free(meshes);
	rjd_slotmap_free(commands);

	IDXGISwapChain1_Release(context_d3d11->swapchain);
	//ID3D11Context_Release(context_d3d11->device); //TODO?
	ID3D11Device_Release(context_d3d11->device);
	IDXGIAdapter1_Release(context_d3d11->adapter);
	IDXGIFactory4_Release(context_d3d11->factory);
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

	// find the primary output and wait for its vblank
	RECT rect_window = {0};
	if (GetWindowRect(context_d3d11->hwnd, &rect_window) == FALSE) {
		return RJD_RESULT("Failed to get window rect. Check GetLastError() for more information"); // TODO make a GetLastError -> result function
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
				RJD_ASSERT(rect_intersect.bottom >= rect_intersect.top); // TODO remove this when you can test it's supposed to be bot - top
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

	struct rjd_result result = rjd_gfx_translate_hresult(hr);
	return result;
}

// commands
struct rjd_result rjd_gfx_command_buffer_create(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* out)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	struct rjd_gfx_command_buffer_d3d11 buffer_d3d11 = {0};
	HRESULT hr = ID3D11Device_CreateDeferredContext(context_d3d11->device, 0, &buffer_d3d11.deferred_context);

	struct rjd_result result = rjd_gfx_translate_hresult(hr);
	if (rjd_result_isok(result)) {
		rjd_slotmap_insert(context_d3d11->slotmap_command_buffers, buffer_d3d11, &out->handle);
	}

	return result;
}

struct rjd_result rjd_gfx_command_pass_begin(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, const struct rjd_gfx_pass_begin_desc* command)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	struct rjd_gfx_command_buffer_d3d11* buffer_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_command_buffers, cmd_buffer->handle);

	if (buffer_d3d11->render_target) {
		return RJD_RESULT("A render pass has already been started with this command buffer.");
	}

	if (rjd_gfx_texture_isbackbuffer(command->render_target)) {
		ID3D11Texture2D* texture_backbuffer = NULL;
		HRESULT hr = IDXGISwapChain_GetBuffer(context_d3d11->swapchain, 0, &IID_ID3D11Texture2D, (void**)&texture_backbuffer);
		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}

		// TODO make sure the texture -> resource cast is safe
		hr = ID3D11Device_CreateRenderTargetView(context_d3d11->device, (ID3D11Resource*)texture_backbuffer, NULL, &buffer_d3d11->render_target);

		// TODO release texture2d?

		if (FAILED(hr)) {
			return rjd_gfx_translate_hresult(hr);
		}
	} else {
		RJD_ASSERTFAIL("unimplemented"); // TODO
	}

	struct rjd_gfx_rgba rgba_backbuffer = rjd_gfx_format_value_to_rgba(command->clear_color);
	ID3D11DeviceContext_ClearRenderTargetView(buffer_d3d11->deferred_context, buffer_d3d11->render_target, rgba_backbuffer.v);

	//ID3D11DeviceContext_ClearDepthStencilView(buffer_d3d11->deferred_context, buffer_d3d11->depth_stencil, color); // TODO

	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_command_pass_draw(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, const struct rjd_gfx_pass_draw_desc* command)
{
	RJD_UNUSED_PARAM(context);
	RJD_UNUSED_PARAM(cmd_buffer);
	RJD_UNUSED_PARAM(command);
	return RJD_RESULT("unimplemented");
}

struct rjd_result rjd_gfx_command_buffer_commit(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	struct rjd_gfx_command_buffer_d3d11* buffer_d3d11 = rjd_slotmap_get(context_d3d11->slotmap_command_buffers, cmd_buffer->handle);

	ID3D11CommandList* commandlist = NULL;
	HRESULT hr = ID3D11DeviceContext_FinishCommandList(buffer_d3d11->deferred_context, FALSE, &commandlist);
	if (FAILED(hr)) {
		return rjd_gfx_translate_hresult(hr);
	}

	ID3D11DeviceContext_ExecuteCommandList(context_d3d11->context, commandlist, FALSE);
	ID3D11CommandList_Release(commandlist); // TODO needed?

	return RJD_RESULT_OK();
}

// resources
struct rjd_result rjd_gfx_texture_create(struct rjd_gfx_context* context, struct rjd_gfx_texture* out, struct rjd_gfx_texture_desc desc)
{
	RJD_UNUSED_PARAM(context);
	RJD_UNUSED_PARAM(out);
	RJD_UNUSED_PARAM(desc);
	return RJD_RESULT("todo");
}

void rjd_gfx_texture_destroy(struct rjd_gfx_context* context, struct rjd_gfx_texture* texture)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	rjd_gfx_texture_destroy_d3d11(context_d3d11, texture->handle);
}

struct rjd_result rjd_gfx_shader_create(struct rjd_gfx_context* context, struct rjd_gfx_shader* out, struct rjd_gfx_shader_desc desc)
{
	RJD_UNUSED_PARAM(context);
	RJD_UNUSED_PARAM(out);
	RJD_UNUSED_PARAM(desc);
	return RJD_RESULT("todo");
}

void rjd_gfx_shader_destroy(struct rjd_gfx_context* context, struct rjd_gfx_shader* shader)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	rjd_gfx_shader_destroy_d3d11(context_d3d11, shader->handle);
}

struct rjd_result rjd_gfx_pipeline_state_create(struct rjd_gfx_context* context, struct rjd_gfx_pipeline_state* out, struct rjd_gfx_pipeline_state_desc desc)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	struct rjd_gfx_vertex_format_attribute* vertex_attributes_clone = 
		rjd_array_alloc(struct rjd_gfx_vertex_format_attribute, desc.count_vertex_attributes, context_d3d11->allocator);
	rjd_array_resize(vertex_attributes_clone, desc.count_vertex_attributes);
	for (size_t i = 0; i < desc.count_vertex_attributes; ++i) {
		vertex_attributes_clone[i] = desc.vertex_attributes[i];
	}

	struct rjd_gfx_pipeline_state_d3d11 state_d3d11 = 
	{
		//.debug_name = desc.debug_name, // TODO deep copy
		.shader = desc.shader,
		.render_target = desc.render_target,
		.depthstencil_target = desc.depthstencil_target,
		.vertex_attributes = vertex_attributes_clone,
		.depth_compare = desc.depth_compare,
	};
	
	rjd_slotmap_insert(context_d3d11->slotmap_pipeline_states, state_d3d11, &out->handle);

	return RJD_RESULT_OK();
}

void rjd_gfx_pipeline_state_destroy(struct rjd_gfx_context* context, struct rjd_gfx_pipeline_state* pipeline_state)
{
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	rjd_gfx_pipeline_state_destroy_d3d11(context_d3d11, pipeline_state->handle);
}

struct rjd_result rjd_gfx_mesh_create_vertexed(struct rjd_gfx_context* context, struct rjd_gfx_mesh* out, struct rjd_gfx_mesh_vertexed_desc desc, struct rjd_mem_allocator* allocator)
{
	RJD_UNUSED_PARAM(context);
	RJD_UNUSED_PARAM(out);
	RJD_UNUSED_PARAM(desc);
	RJD_UNUSED_PARAM(allocator);
	return RJD_RESULT("todo");
}
//struct rjd_result rjd_gfx_mesh_create_indexed(struct rjd_gfx_context* context, struct rjd_gfx_mesh* out, struct rjd_gfx_mesh_indexed_desc desc);
struct rjd_result rjd_gfx_mesh_modify(struct rjd_gfx_context* context, struct rjd_gfx_mesh* mesh, uint32_t buffer_index, uint32_t offset, void* data, uint32_t length)
{
	RJD_UNUSED_PARAM(context);
	RJD_UNUSED_PARAM(mesh);
	RJD_UNUSED_PARAM(buffer_index);
	RJD_UNUSED_PARAM(offset);
	RJD_UNUSED_PARAM(data);
	RJD_UNUSED_PARAM(length);
	return RJD_RESULT("todo");
}

void rjd_gfx_mesh_destroy(struct rjd_gfx_context* context, struct rjd_gfx_mesh* mesh)
{
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

	return RJD_RESULT("Failed. TODO more info.");
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

static struct rjd_gfx_rgba rjd_gfx_format_value_to_rgba(struct rjd_gfx_format_value value)
{
	struct rjd_gfx_rgba out = {0};
	switch (value.type)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA:
			out.v[0] = value.color_u8_rgba[0];
			out.v[1] = value.color_u8_rgba[1];
			out.v[2] = value.color_u8_rgba[2];
			out.v[3] = value.color_u8_rgba[3];
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

static bool rjd_gfx_texture_isbackbuffer(struct rjd_gfx_texture texture)
{
	return	texture.handle.salt == RJD_GFX_TEXTURE_BACKBUFFER.handle.salt &&
			texture.handle.index == RJD_GFX_TEXTURE_BACKBUFFER.handle.index;
}

static inline void rjd_gfx_texture_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot)
{
	rjd_slotmap_erase(context->slotmap_textures, slot);
}

static inline void rjd_gfx_shader_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot)
{
	rjd_slotmap_erase(context->slotmap_shaders, slot);
}

static inline void rjd_gfx_pipeline_state_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot)
{
	struct rjd_gfx_pipeline_state_d3d11* pipeline_state = rjd_slotmap_get(context->slotmap_pipeline_states, slot);
	rjd_array_free(pipeline_state->vertex_attributes);
	rjd_slotmap_erase(context->slotmap_pipeline_states, slot);
}

static inline void rjd_gfx_mesh_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot)
{
	rjd_slotmap_erase(context->slotmap_meshes, slot);
}

static inline void rjd_gfx_command_buffer_destroy_d3d11(struct rjd_gfx_context_d3d11* context, struct rjd_slot slot)
{
	rjd_slotmap_erase(context->slotmap_command_buffers, slot);
}

#endif
