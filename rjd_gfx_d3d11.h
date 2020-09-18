#pragma once

#if 0

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

////////////////////////////////////////////////////////////////////////////////
// Local helpers

struct rjd_gfx_context_d3d11
{
	ID3D11Device* device;
	IDXGIAdapter1* adapter;
	IDXGIFactory* factory;
	//ID3D11Debug* debug;
};
RJD_STATIC_ASSERT(sizeof(rjd_gfx_context_d3d11) <= sizeof(rjd_gfx_context));


////////////////////////////////////////////////////////////////////////////////
// interface implementation

struct rjd_result rjd_gfx_context_create(struct rjd_gfx_context* out, struct rjd_gfx_context_desc desc)
{
	RJD_ASSERT(out);
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)out;

	//{
	//	if (SUCCEEDED(D3D11GetDebugInterface(&IID_ID3D11Debug, &out->debug))) {
	//		ID3D11Debug_EnableDebugLayer(out->debug);
	//	} else {
	//		printf("Failed to get debug interface\n");
	//		return false;
	//	}
	//}

	{
		UINT flags = 0;
		flags |= DXGI_CREATE_FACTORY_DEBUG; // TODO make this optional

		HRESULT hr = CreateDXGIFactory2(flags, &IID_IDXGIFactory4, &context_d3d11->factory);
		if (!SUCCEEDED(hr)) {
			printf("Failed to create factory: %u\n", hr);
			return false;
		}
	}


////////////////////////////////////////////////////////////////////////////////
// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return false;
	}

	// Release the display mode list.
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if(m_vsync_enabled)
	{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if(fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;
////////////////////////////////////////////////////////////////////////////////


	const UINT flags = 
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | // enable compatibility with Direct2D.
		D3D11_CREATE_DEVICE_DEBUG | // TODO make this optional
		0;

	const D3D_FEATURE_LEVEL feature_levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	struct rjd_result result = RJD_RESULT("No hardware adapter found. Does this machine have a GPU?");
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != IDXGIFactory1_EnumAdapters1(context_d3d11->factory, i, &context_d3d11->adapter); ++i) {
		DXGI_ADAPTER_DESC1 desc_adapter;
		IDXGIAdapter1_GetDesc1(context_d3d11->adapter, &desc_adapter);
		if (desc_adapter.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			context_d3d11->adapter = NULL;
			continue;
		}

		// desc_adapter.DedicatedVideoMemory
		// desc_adapter.Description // wchar_t* name
	}

	const HRESULT hr_device = D3D11CreateDevice(
		context_d3d11->adapter,
		D3D_DRIVER_TYPE_HARDWARE,
		flags,
		feature_levels,
		rjd_countof(feature_levels),
		D3D11_SDK_VERSION,
		&context_d3d11->device,
		NULL,
		&context_d3d11->context);

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

	{
		DXGI_SWAP_CHAIN_DESC1 desc_swapchain = {
			.Width = 0, // auto-detect
			.Height = 0, // auto-detect
			.Format = rjd_gfx_format_to_dxgi(desc.backbuffer_color_format),
			.Stereo = false,
			.SampleDesc = NULL, // TODO
			.BufferUsage = DXGI_USAGE_BACK_BUFFER,
			.BufferCount = 3, // TODO make this configurable. Right now 3 matches the metal defaults.
			.Scaling = DXGI_SCALING_NONE, // Don't stretch the backbuffer to fit the window size. We should handle that.
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD, // flip model has better perf and discard since we don't care to keep it around
			.AlphaMode = DXGI_ALPHA_MODE_STRAIGHT,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING, // needed to turn off vsync
		};

		//DXGI_SWAP_CHAIN_FULLSCREEN_DESC desc_fullscreen = {0};
		//if (desc.is_fullscreen) {
		//}

		HRESULT hr_swap = IDXGIFactory2_CreateSwapChainForHwnd(
			context_d3d11->device,
			desc.hwnd,
			&desc_swapchain,
			desc_fullscreen_p,
			NULL, // don't restrict to a particular output of the adapter
			&context_d3d11->swapchain);
	}

	return result;
}

void rjd_gfx_context_destroy(struct rjd_gfx_context* context)
{
	RJD_ASSERT(context);
	struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;

	ID3D12Device_Release(context_d3d11->device);
	IDXGIAdapter1_Release(context_d3d11->adapter);
	IDXGIFactory4_Release(context_d3d11->factory);
}

bool rjd_gfx_msaa_is_count_supported(const struct rjd_gfx_context* context, uint32_t count); // count is usually: 1,2,4, or 8
{
	RJD_ASSERT(context);
	//struct rjd_gfx_context_d3d11* context_d3d11 = (struct rjd_gfx_context_d3d11*)context;
	//
	RJD_UNUSED_PARAM(count);
	return false;

	//DXGI_FORMAT format = 0; // TODO should probably be passed in? probably needs to match the backbuffer format
	//UINT supported_levels = 0; // TODO not sure what this is. 

	//// struct DXGI_SWAP_CHAIN_DESC // probably has something to do with this?

	//ID3D11Device_CheckMultisampleQualityLevels(context_d3d11->context, format, count, &supported_levels);
	//if (FAILED(hr)) {
	//	return false;
	//}
	//
	//return supported_levels > 0;
}

void rjd_gfx_msaa_set_count(struct rjd_gfx_context* context, uint32_t count)
{
	// TODO
	RJD_UNUSED_PARAM(context);
	RJD_UNUSED_PARAM(count);
}

bool rjd_gfx_vsync_try_enable(struct rjd_gfx_context* context)
{
	RJD_UNUSED_PARAM(context);
	return false; // TODO
}

struct rjd_result rjd_gfx_wait_for_frame_begin(struct rjd_gfx_context* context)
{
}

struct rjd_result rjd_gfx_present(struct rjd_gfx_context* context)
{
	
}

// commands
struct rjd_result rjd_gfx_command_buffer_create(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* out);
struct rjd_result rjd_gfx_command_pass_begin(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, const struct rjd_gfx_pass_begin_desc* command);
struct rjd_result rjd_gfx_command_pass_draw(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, const struct rjd_gfx_pass_draw_desc* command);
struct rjd_result rjd_gfx_command_buffer_commit(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer);

// resources
struct rjd_result rjd_gfx_texture_create(struct rjd_gfx_context* context, struct rjd_gfx_texture* out, struct rjd_gfx_texture_desc desc);
void rjd_gfx_texture_destroy(struct rjd_gfx_context* context, struct rjd_gfx_texture* texture);
struct rjd_result rjd_gfx_shader_create(struct rjd_gfx_context* context, struct rjd_gfx_shader* out, struct rjd_gfx_shader_desc desc);
void rjd_gfx_shader_destroy(struct rjd_gfx_context* context, struct rjd_gfx_shader* shader);
struct rjd_result rjd_gfx_pipeline_state_create(struct rjd_gfx_context* context, struct rjd_gfx_pipeline_state* out, struct rjd_gfx_pipeline_state_desc desc);
void rjd_gfx_pipeline_state_destroy(struct rjd_gfx_context* context, struct rjd_gfx_pipeline_state* pipeline_state);
struct rjd_result rjd_gfx_mesh_create_vertexed(struct rjd_gfx_context* context, struct rjd_gfx_mesh* out, struct rjd_gfx_mesh_vertexed_desc desc, struct rjd_mem_allocator* allocator);
//struct rjd_result rjd_gfx_mesh_create_indexed(struct rjd_gfx_context* context, struct rjd_gfx_mesh* out, struct rjd_gfx_mesh_indexed_desc desc);
struct rjd_result rjd_gfx_mesh_modify(struct rjd_gfx_context* context, struct rjd_gfx_mesh* mesh, uint32_t buffer_index, uint32_t offset, void* data, uint32_t length);
void rjd_gfx_mesh_destroy(struct rjd_gfx_context* context, struct rjd_gfx_mesh* mesh);





////////////////////////////////////////////////////////////////////////////////
// local implementation

#endif
