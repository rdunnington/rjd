#pragma once

#define RJD_WINDOW_H 1

struct rjd_window;
struct rjd_window_environment;

typedef void (*rjd_window_environment_init_func)(const struct rjd_window_environment* env);
typedef void (*rjd_window_on_init_func)(struct rjd_window* window, const struct rjd_window_environment* env);
typedef void (*rjd_window_on_update_func)(struct rjd_window* window, const struct rjd_window_environment* env);
typedef void (*rjd_window_on_close_func)(struct rjd_window* window, const struct rjd_window_environment* env);

struct rjd_window_data_win32
{
#if RJD_PLATFORM_WINDOWS
	HINSTANCE handle_instance
#else
	char unused;
#endif
};

struct rjd_window_environment
{
	void* userdata;
	const char** argv;
	int argc;

	struct rjd_window_data_win32 win32;
};

struct rjd_window_size
{
	uint16_t width;
	uint16_t height;
};

struct rjd_window_desc
{
    const char* title;
	struct rjd_window_size requested_size;
    struct rjd_window_environment env;
    
    rjd_window_on_init_func init_func;
    rjd_window_on_update_func update_func;
    rjd_window_on_close_func close_func;
    
    struct rjd_window_data_win32 win32;
};

struct rjd_window
{
	char impl[40];
};

void rjd_window_enter_windowed_environment(struct rjd_window_environment env, rjd_window_environment_init_func init_func);
struct rjd_result rjd_window_create(struct rjd_window* out, struct rjd_window_desc desc);
void rjd_window_runloop(struct rjd_window* window);
struct rjd_window_size rjd_window_size_get(const struct rjd_window* window);
void rjd_window_close(struct rjd_window* window);

#if RJD_PLATFORM_WINDOWS
	HWND rjd_window_win32_get_hwnd(const struct rjd_window* window);
#elif RJD_PLATFORM_OSX
	#if RJD_LANG_OBJC
		@class MTKView;
		RJD_STATIC_ASSERT(sizeof(MTKView*) == sizeof(void*));
	#else
		typedef void MTKView;
	#endif
	MTKView* rjd_window_osx_get_mtkview(const struct rjd_window* window);
#endif

////////////////////////////////////////////////////////////////////////////////
// implementation

#if RJD_IMPL

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// windows os

#if RJD_PLATFORM_WINDOWS

static uint32_t global_window_count = 0;

struct rjd_window_win32
{
	rjd_window_on_init_func init_func;
	rjd_window_on_update_func update_func;
	rjd_window_on_close_func close_func;
	HWND handle;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_window) >= sizeof(struct rjd_window_win32));

static LRESULT CALLBACK WindowProc(HWND handle_window, UINT msg, WPARAM wparam, LPARAM lparam);

void rjd_window_enter_windowed_environment(struct rjd_window_environment env, rjd_window_environment_init_func init_func)
{
	if (init_func) {
		init_func(env);
	}

	while (global_window_count > 0)
	{
		// other threads could be running their own window loops, so just wait until
		// all of them are closed before exiting
	}
}

struct rjd_result rjd_window_create(struct rjd_window* out, struct rjd_window_desc desc)
{
	if (desc.title == NULL) {
		desc.title = "";
	}

	WNDCLASSEX window_class = { 0 };
	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = WindowProc;
	window_class.hInstance = handle_instance;
	window_class.lpszClassName = "EngineWindowClass";
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	//window_class.hIcon = LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	if (!RegisterClassEx(&window_class))
	{
		return RJD_RESULT("Failed to create window class");
	}

	const DWORD window_style = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU;
	RECT window_rect = {0, 0, desc.width, desc.height};
	if (!AdjustWindowRect(&window_rect, window_style, FALSE))
	{
		return RJD_RESULT("Failed to adjust window rect");
	} 

	HWND window_handle = CreateWindowEx(0,
		"EngineWindowClass",
		desc.title,
		window_style, 
		1024, 80,
		window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
		NULL,
		NULL,
		handle_instance,
		NULL);
	if (!window_handle)
	{
		return RJD_RESULT("Failed to create window from class");
	}

	struct rjd_window_win32* window_win32 = (struct rjd_window_win32*)out;
	window_win32->handle = window_handle;

	if (desc.init_func) {
		desc.init_func(out, env);
	}

	++global_window_count;
}

void rjd_window_runloop(struct rjd_window* window, struct rjd_window_environment env)
{
	struct rjd_window_win32* window_win32 = (struct rjd_window_win32*)window;
	if (window_win32->init_func) {
		window_win32->init_func(env);
	}

	bool running = true;
	while (running)
	{
		MSG msg = {0};
		while (PeekMessage(&msg, window_win32->handle, 0, 0, PM_REMOVE))
		{
			// TODO support running multiple windows in the same thread?
			if (msg.message == WM_DESTROY)
			{
				running = false;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if (running && window_win32->update_func) {
			window_win32->update_func(window, env);
		}
	}

	if (window_win32->close_func) {
		window_win32->close_func(window, env);
	}
}

struct rjd_window_size rjd_window_size_get(const struct rjd_window* window)
{
	#error Unimplemented
}

void rjd_window_close(struct rjd_window* window)
{
	struct rjd_window_win32* window_win32 = (struct rjd_window_win32*)window;
	DestroyWindow(window->handle);
}

HWND rjd_window_win32_get_hwnd(const struct rjd_window* window)
{
	return window->handle;
}

////////////////////////////////////////////////////////////////////////////////
// Local helpers

LRESULT CALLBACK WindowProc(HWND handle_window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    const POINT kMinSize = {1, 1};
    switch (msg)
    {
    //case WM_CLOSE:
    //    PostQuitMessage(0);
    //    break;
    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lparam)->ptMinTrackSize = kMinSize;
        break;
		// TODO forward resize message
    //case WM_SIZE:
	//	//width = LOWORD(lparam);
	//	//height = HIWORD(lparam);
	//	//glViewport(0, 0, g_window_size.width, g_window_size.height);
    //    break;
    }
    return DefWindowProc(handle_window, msg, wparam, lparam);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// osx os

#elif RJD_PLATFORM_OSX

#if !RJD_LANG_OBJC
    #error "rjd_window.h must be implemented in a .m file on OSX"
#endif

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#import <MetalKit/MetalKit.h>

struct rjd_window_osx
{
    NSWindow* window;
    MTKView* view;
    
    rjd_window_on_init_func init_func;
    rjd_window_on_update_func update_func;
    rjd_window_on_close_func close_func;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_window) >= sizeof(struct rjd_window_osx));

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (retain) NSWindow *window;
-(instancetype)initWithEnvFunc:(rjd_window_environment_init_func)func env:(struct rjd_window_environment)env;
@end

@interface GameViewController : NSViewController
-(instancetype)initWithWidth:(uint16_t)width height:(uint16_t)height window:(struct rjd_window*)window env:(struct rjd_window_environment)env;
@end

@interface Renderer : NSObject <MTKViewDelegate>
-(instancetype)initWithWindow:(struct rjd_window*)window env:(struct rjd_window_environment)env;
@end

////////////////////////////////////////////////////////////////////////////////
// interface implementation

void rjd_window_enter_windowed_environment(struct rjd_window_environment env, rjd_window_environment_init_func init_func)
{
    AppDelegate* delegate = [[AppDelegate alloc] initWithEnvFunc:init_func env:env];
    NSApplication* app = [NSApplication sharedApplication];
    [app setDelegate:delegate];
    [app run];
}

struct rjd_result rjd_window_create(struct rjd_window* out, struct rjd_window_desc desc)
{
    if (desc.title == NULL) {
        desc.title = "";
    }
    
    NSRect rect = {0};
    rect.size.height = 320;
    rect.size.width = 480;
    
    NSWindowStyleMask style =   NSWindowStyleMaskResizable |
                                NSWindowStyleMaskTitled    |
                                NSWindowStyleMaskClosable  |
                                NSWindowStyleMaskMiniaturizable;

    NSWindow* window = [[NSWindow alloc] initWithContentRect:rect styleMask:style backing:NSBackingStoreBuffered defer:false];
    window.title = [NSString stringWithUTF8String:desc.title];
    
    struct rjd_window_size size = desc.requested_size;

    GameViewController* viewController = [[GameViewController alloc]
                                          initWithWidth:size.width height:size.height window:out env:desc.env];
    MTKView* view = [[MTKView alloc] initWithFrame:rect device:MTLCreateSystemDefaultDevice()];
    
    struct rjd_window_osx* window_osx = (struct rjd_window_osx*)out;
    window_osx->window = window;
    window_osx->init_func = desc.init_func;
    window_osx->update_func = desc.update_func;
    window_osx->close_func = desc.close_func;
    window_osx->view = view;

    viewController.view = view;
    [viewController loadView];
    window.contentViewController = viewController;

    [window makeKeyAndOrderFront:nil];

    return RJD_RESULT_OK();
}

void rjd_window_runloop(struct rjd_window* window)
{
	// no-op since OSX is event driven
	RJD_UNUSED_PARAM(window);
}

struct rjd_window_size rjd_window_size_get(const struct rjd_window* window)
{
	const struct rjd_window_osx* window_osx = (const struct rjd_window_osx*)window;
	CGSize size = window_osx->view.drawableSize;
	struct rjd_window_size windowsize = {
		.width = size.width,
		.height = size.height,
	};
	return windowsize;
}

MTKView* rjd_window_osx_get_mtkview(const struct rjd_window* window)
{
    const struct rjd_window_osx* window_osx = (const struct rjd_window_osx*)window;
	return window_osx->view;
}

////////////////////////////////////////////////////////////////////////////////
// local helpers

@implementation AppDelegate
{
    rjd_window_environment_init_func init_func;
	struct rjd_window_environment env;
}

-(instancetype)initWithEnvFunc:(rjd_window_environment_init_func)func env:(struct rjd_window_environment)_env
{
    if (self = [super init]) {
        self->init_func = func;
		self->env = _env;
    }
    return self;
}

-(void)applicationDidFinishLaunching:(NSNotification*)notification
{
	RJD_UNUSED_PARAM(notification);

	if (self->init_func) {
		self->init_func(&self->env);
	}
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
	RJD_UNUSED_PARAM(sender);

    return YES;
}

@end

@implementation GameViewController
{
    Renderer* renderer;
    uint16_t width;
    uint16_t height;
    struct rjd_window* window;
    struct rjd_window_environment env;
}

-(instancetype)initWithWidth:(uint16_t)_width height:(uint16_t)_height window:(struct rjd_window*)_window env:(struct rjd_window_environment)_env
{
    if (self = [super init]) {
        self->width = _width;
        self->height = _height;
        self->window = _window;
        self->env = _env;
    }
    return self;
}

-(void)loadView
{
    MTKView* view = (MTKView*)self.view;
    if(!view.device)
    {
        NSLog(@"Metal is not supported on this device");
        self.view = [[NSView alloc] initWithFrame:self.view.frame];
        return;
    }
    
    struct rjd_window_osx* window_osx = (struct rjd_window_osx*)self->window;
    if (window_osx->init_func) {
        window_osx->init_func(self->window, &self->env);
    }

    // We need to hold a strong reference to the Renderer or it will go out of scope
    // after this function and be destroyed. MTKView.delegate is a weak reference.
    self->renderer = [[Renderer alloc] initWithWindow:self->window env:self->env];
    [self->renderer mtkView:view drawableSizeWillChange:view.bounds.size];
    view.delegate = renderer;
    self.view = view;
}
@end

@implementation Renderer
{
    id<MTLDevice> device;
	struct rjd_window* window;
	struct rjd_window_environment env;
}

-(instancetype)initWithWindow:(struct rjd_window*)_window env:(struct rjd_window_environment)_env
{
	if (self = [super init]) {
		self->window = _window;
		self->env = _env;
        
        struct rjd_window_osx* window_osx = (struct rjd_window_osx*)window;

        [[NSNotificationCenter defaultCenter]    addObserver:self
                                                selector:@selector(windowWillClose:)
                                                name:NSWindowWillCloseNotification
                                                object:window_osx->window];
	}
    
    return self;
}

-(void)drawInMTKView:(MTKView*)view
{
	RJD_UNUSED_PARAM(view);

    struct rjd_window_osx* window_osx = (struct rjd_window_osx*)self->window;
	if (window_osx->update_func) {
		window_osx->update_func(self->window, &self->env);
	}
}

-(void)windowWillClose:(NSNotification*)notification
{
	RJD_UNUSED_PARAM(notification);

    struct rjd_window_osx* window_osx = (struct rjd_window_osx*)window;

    [[NSNotificationCenter defaultCenter] removeObserver:self name:NSWindowWillCloseNotification object:window_osx->window];

    if (window_osx->close_func) {
        window_osx->close_func(self->window, &self->env);
    }
}

-(void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size
{
	RJD_UNUSED_PARAM(view);
	RJD_UNUSED_PARAM(size);
}
@end

#else
	#error Unsupported platform.
#endif

#endif // RJD_IMPL
