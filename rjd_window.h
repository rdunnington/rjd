#pragma once

#define RJD_WINDOW_H 1

struct rjd_window;
struct rjd_window_environment;

typedef void rjd_window_environment_init_func(const struct rjd_window_environment* env);
typedef void rjd_window_environment_close_func(const struct rjd_window_environment* env);
typedef void rjd_window_on_init_func(struct rjd_window* window, const struct rjd_window_environment* env);
typedef bool rjd_window_on_update_func(struct rjd_window* window, const struct rjd_window_environment* env);
typedef void rjd_window_on_close_func(struct rjd_window* window, const struct rjd_window_environment* env);

// Note that we use void* instead of the win32 types HINSTANCE and HWND to avoid taking a dependency 
// on a windows header.

struct rjd_window_data_win32
{
#if RJD_PLATFORM_WINDOWS
	void* hinstance; // HINSTANCE
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
    
    rjd_window_on_init_func* init_func;
    rjd_window_on_update_func* update_func;
    rjd_window_on_close_func* close_func;
};

struct rjd_window
{
	char impl[64];
};

void rjd_window_enter_windowed_environment(struct rjd_window_environment env, rjd_window_environment_init_func* init_func, rjd_window_environment_close_func* close_func);
struct rjd_result rjd_window_create(struct rjd_window* out, struct rjd_window_desc desc);
void rjd_window_runloop(struct rjd_window* window);
struct rjd_window_size rjd_window_size_get(const struct rjd_window* window);
void rjd_window_close(struct rjd_window* window);

#if RJD_PLATFORM_WINDOWS
	void* rjd_window_win32_get_hwnd(const struct rjd_window* window);
#elif RJD_PLATFORM_OSX
	#if RJD_LANG_OBJC
		@class MTKView;
		RJD_STATIC_ASSERT(sizeof(MTKView*) == sizeof(void*));
		@class NSWindow;
		RJD_STATIC_ASSERT(sizeof(NSWindow*) == sizeof(void*));
	#else
		typedef void MTKView;
		typedef void NSWindow;
	#endif
	MTKView* rjd_window_osx_get_mtkview(const struct rjd_window* window);
	NSWindow* rjd_window_osx_get_nswindow(const struct rjd_window* window);
#endif

////////////////////////////////////////////////////////////////////////////////
// implementation

#if RJD_IMPL

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// windows os

static struct rjd_atomic_uint32 global_window_count = {0};

#if RJD_PLATFORM_WINDOWS


struct rjd_window_win32
{
	struct rjd_window_environment env;
	rjd_window_on_init_func* init_func;
	rjd_window_on_update_func* update_func;
	rjd_window_on_close_func* close_func;
	void* hwnd; // HWND
};
RJD_STATIC_ASSERT(sizeof(struct rjd_window) >= sizeof(struct rjd_window_win32));

static LRESULT CALLBACK WindowProc(HWND handle_window, UINT msg, WPARAM wparam, LPARAM lparam);

void rjd_window_enter_windowed_environment(struct rjd_window_environment env, rjd_window_environment_init_func* init_func, rjd_window_environment_close_func* close_func)
{
	if (init_func) {
		init_func(&env);
	}

	while (rjd_atomic_uint32_get(&global_window_count) > 0)
	{
		// other threads could be running their own window loops, so just wait until
		// all of them are closed before exiting
	}

	if (close_func) {
		close_func(&env);
	}
}

struct rjd_result rjd_window_create(struct rjd_window* out, struct rjd_window_desc desc)
{
	if (desc.title == NULL) {
		desc.title = "";
	}

	static bool s_did_register_window_class = false;
	if (!s_did_register_window_class) {
		s_did_register_window_class = true;

		WNDCLASSEX window_class = { 0 };
		window_class.cbSize = sizeof(WNDCLASSEX);
		window_class.style = CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc = WindowProc;
		window_class.hInstance = desc.env.win32.hinstance;
		window_class.lpszClassName = "rjd_window_class";
		window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		//window_class.hIcon = LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
		if (!RegisterClassEx(&window_class))
		{
			return RJD_RESULT("Failed to create window class");
		}
	}

	const DWORD window_style = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU;
	RECT window_rect = {0, 0, desc.requested_size.width, desc.requested_size.height};
	if (!AdjustWindowRect(&window_rect, window_style, FALSE))
	{
		return RJD_RESULT("Failed to adjust window rect");
	} 

	HWND hwnd = CreateWindowEx(0,
		"rjd_window_class",
		desc.title,
		window_style, 
		1024, 80,
		window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
		NULL,
		NULL,
		desc.env.win32.hinstance,
		NULL);
	if (!hwnd)
	{
		return RJD_RESULT("Failed to create window from class");
	}

	struct rjd_window_win32* window_win32 = (struct rjd_window_win32*)out;
	window_win32->env = desc.env;
	window_win32->init_func = desc.init_func;
	window_win32->update_func = desc.update_func;
	window_win32->close_func = desc.close_func;
	window_win32->hwnd = hwnd;	

	return RJD_RESULT_OK();
}

void rjd_window_runloop(struct rjd_window* window)
{
	rjd_atomic_uint32_inc(&global_window_count);

	struct rjd_window_win32* window_win32 = (struct rjd_window_win32*)window;
	if (window_win32->init_func) {
		window_win32->init_func(window, &window_win32->env);
	}

	// TODO support running multiple windows in the same thread?
	bool running = true;
	while (running)
	{
		MSG msg = {0};
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				running = false;
			}
		}

		if (running && window_win32->update_func) {
			running = window_win32->update_func(window, &window_win32->env);
		}
	}

	if (window_win32->close_func) {
		window_win32->close_func(window, &window_win32->env);
	}

	if (IsWindow(window_win32->hwnd)) {
		DestroyWindow(window_win32->hwnd);
	}

	rjd_atomic_uint32_dec(&global_window_count);
}

struct rjd_window_size rjd_window_size_get(const struct rjd_window* window)
{
	struct rjd_window_win32* window_win32 = (struct rjd_window_win32*)window;

	RECT rect = {0};
	GetClientRect(window_win32->hwnd, &rect);

	uint32_t width = rect.right - rect.left;
	uint32_t height = rect.bottom - rect.top;

	RJD_ASSERT(width <= UINT16_MAX);
	RJD_ASSERT(height <= UINT16_MAX);

	struct rjd_window_size size = 
	{
		.width = (uint16_t)width,
		.height = (uint16_t)height,
	};
	return size;
}

void rjd_window_close(struct rjd_window* window)
{
	struct rjd_window_win32* window_win32 = (struct rjd_window_win32*)window;
	PostMessageA(window_win32->hwnd, WM_CLOSE, 0, 0);
}

void* rjd_window_win32_get_hwnd(const struct rjd_window* window)
{
	struct rjd_window_win32* window_win32 = (struct rjd_window_win32*)window;
	return window_win32->hwnd;
}

////////////////////////////////////////////////////////////////////////////////
// Local helpers

LRESULT CALLBACK WindowProc(HWND handle_window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    const POINT kMinSize = {1, 1};
    switch (msg)
    {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lparam)->ptMinTrackSize = kMinSize;
        break;
		// TODO forward resize message
    //case WM_SIZE:
	//	//width = LOWORD(lparam);
	//	//height = HIWORD(lparam);
	//	//glViewport(0, 0, g_window_size.width, g_window_size.height);
    //    break;
	default:
		return DefWindowProc(handle_window, msg, wparam, lparam);
	}

	return 0;
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
    NSWindow* nswindow;
    MTKView* view;
    
    rjd_window_on_init_func* init_func;
    rjd_window_on_update_func* update_func;
    rjd_window_on_close_func* close_func;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_window) >= sizeof(struct rjd_window_osx));

bool s_is_app_initialized = false;

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (retain) NSWindow *window;
-(instancetype)initWithEnvFunc:(rjd_window_environment_init_func*)init_func closeFunc:(rjd_window_environment_close_func*)close_func env:(struct rjd_window_environment)env;
@end

@interface CustomViewController : NSViewController
-(instancetype)initWithWidth:(uint16_t)width height:(uint16_t)height window:(struct rjd_window*)window env:(struct rjd_window_environment)env;
@end

@interface CustomWindow : NSWindow
@end

@interface Renderer : NSObject <MTKViewDelegate>
-(instancetype)initWithWindow:(struct rjd_window*)window env:(struct rjd_window_environment)env;
@end

////////////////////////////////////////////////////////////////////////////////
// interface implementation

void rjd_window_enter_windowed_environment(struct rjd_window_environment env, rjd_window_environment_init_func* init_func, rjd_window_environment_close_func* close_func)
{
	NSApplicationLoad();
    AppDelegate* delegate = [[AppDelegate alloc] initWithEnvFunc:init_func closeFunc:close_func env:env];
    NSApplication* app = [NSApplication sharedApplication];
    [app setDelegate:delegate];
	[app activateIgnoringOtherApps:YES];
    
    // The applicationDidFinishLaunching notification isn't sent multiple times. We need to keep track
    // to ensure the init_func() is called upon reentry to this function
    if (s_is_app_initialized && init_func) {
        init_func(&env);
    }
    [app run];
    if (close_func) {
    	close_func(&env);
    }
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

    NSWindow* nswindow = [[CustomWindow alloc] initWithContentRect:rect styleMask:style backing:NSBackingStoreBuffered defer:false];
    nswindow.title = [NSString stringWithUTF8String:desc.title];
    
    struct rjd_window_size size = desc.requested_size;

    CustomViewController* viewController = [[CustomViewController alloc]
                                          initWithWidth:size.width height:size.height window:out env:desc.env];
    MTKView* view = [[MTKView alloc] initWithFrame:rect device:MTLCreateSystemDefaultDevice()];

    viewController.view = view;
    [viewController loadView];
    nswindow.contentViewController = viewController;

	RJD_ASSERT(nswindow.canBecomeKeyWindow == YES);
	RJD_ASSERT(nswindow.canBecomeMainWindow == YES);
    [nswindow makeKeyAndOrderFront:nil];

	rjd_atomic_uint32_inc(&global_window_count);

    struct rjd_window_osx* window_osx = (struct rjd_window_osx*)out;
    window_osx->nswindow = nswindow;
    window_osx->init_func = desc.init_func;
    window_osx->update_func = desc.update_func;
    window_osx->close_func = desc.close_func;
    window_osx->view = view;

    if (window_osx->init_func) {
        window_osx->init_func((struct rjd_window*)window_osx, &desc.env);
    }

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

void rjd_window_close(struct rjd_window* window)
{
	const struct rjd_window_osx* window_osx = (const struct rjd_window_osx*)window;
	[window_osx->nswindow close];
	rjd_atomic_uint32_dec(&global_window_count);
}

MTKView* rjd_window_osx_get_mtkview(const struct rjd_window* window)
{
    const struct rjd_window_osx* window_osx = (const struct rjd_window_osx*)window;
	return window_osx->view;
}

NSWindow* rjd_window_osx_get_nswindow(const struct rjd_window* window)
{
    const struct rjd_window_osx* window_osx = (const struct rjd_window_osx*)window;
	return window_osx->nswindow;
}

////////////////////////////////////////////////////////////////////////////////
// local helpers

@implementation AppDelegate
{
    rjd_window_environment_init_func* init_func;
    rjd_window_environment_close_func* close_func;
	struct rjd_window_environment env;
}

-(instancetype)initWithEnvFunc:(rjd_window_environment_init_func*)init_func closeFunc:(rjd_window_environment_close_func*)close_func env:(struct rjd_window_environment)_env
{
    if (self = [super init]) {
        self->init_func = init_func;
        self->close_func = close_func;
		self->env = _env;
    }
    return self;
}

-(void)applicationDidFinishLaunching:(NSNotification*)notification
{
	RJD_UNUSED_PARAM(notification);
    
    NSMenuItem* testItem = [[NSMenuItem alloc] initWithTitle:@"TestItem" action:nil keyEquivalent:@""];

    NSMenu* menu = [[NSMenu alloc] initWithTitle:@"TestMenu"];
    [menu addItem:testItem];
    NSApplication.sharedApplication.mainMenu = menu;
    
    s_is_app_initialized = true;

	if (self->init_func) {
		self->init_func(&self->env);
	}
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
	RJD_UNUSED_PARAM(sender);
    
	// The NSApplication event loop will only check the stop flag *after* it handles an event.
    [sender stop:nil];
	NSEvent* force_handler_event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                  location:NSZeroPoint
             modifierFlags:0
                 timestamp:0
              windowNumber:0
                   context:nil
                   subtype:0
                     data1:0
                     data2:0];
    [NSApp postEvent:force_handler_event atStart:TRUE];

    return NO;
}

@end

@implementation CustomViewController
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

    // We need to hold a strong reference to the Renderer or it will go out of scope
    // after this function and be destroyed. MTKView.delegate is a weak reference.
    self->renderer = [[Renderer alloc] initWithWindow:self->window env:self->env];
    [self->renderer mtkView:view drawableSizeWillChange:view.bounds.size];
    view.delegate = renderer;
    self.view = view;
}
@end

@implementation CustomWindow
{
}

-(BOOL)canBecomeMainWindow
{
	return YES;
}

-(BOOL)canBecomeKeyWindow
{
	return YES;
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
                                                object:window_osx->nswindow];
	}
    
    return self;
}

-(void)drawInMTKView:(MTKView*)view
{
	RJD_UNUSED_PARAM(view);

    struct rjd_window_osx* window_osx = (struct rjd_window_osx*)self->window;
	if (window_osx->update_func) {
		if (window_osx->update_func(self->window, &self->env) == false) {
			rjd_window_close(self->window);
		}
	}
}

-(void)windowWillClose:(NSNotification*)notification
{
	RJD_UNUSED_PARAM(notification);

    struct rjd_window_osx* window_osx = (struct rjd_window_osx*)window;

    [[NSNotificationCenter defaultCenter] removeObserver:self name:NSWindowWillCloseNotification object:window_osx->nswindow];

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
