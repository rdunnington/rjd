#pragma once

#define RJD_INPUT_H 1

struct rjd_input
{
	char impl[32];
};

enum rjd_input_keyboard
{
	RJD_INPUT_KEYBOARD_LEFTCONTROL,
	RJD_INPUT_KEYBOARD_RIGHTCONTROL,
	RJD_INPUT_KEYBOARD_LEFTSHIFT,
	RJD_INPUT_KEYBOARD_RIGHTSHIFT,
	RJD_INPUT_KEYBOARD_LEFTALT,
	RJD_INPUT_KEYBOARD_RIGHTALT,
	RJD_INPUT_KEYBOARD_ESCAPE,
	RJD_INPUT_KEYBOARD_UTF8_BEGIN, // Add the utf8 codepoint to this value to check if the corresponding key is down
};

enum rjd_input_mouse
{
	RJD_INPUT_MOUSE_XMOVEDELTA,
	RJD_INPUT_MOUSE_YMOVEDELTA,
	RJD_INPUT_MOUSE_SCROLLWHEEL,
	RJD_INPUT_MOUSE_BUTTON_BEGIN, // Add the index of the desired button to this value
};

//const enum rjd_input_mouse RJD_INPUT_MOUSE_BUTTON_LEFT = RJD_INPUT_MOUSE_BUTTON_BEGIN + 0;
//const enum rjd_input_mouse RJD_INPUT_MOUSE_BUTTON_RIGHT = RJD_INPUT_MOUSE_BUTTON_BEGIN + 1;
//const enum rjd_input_mouse RJD_INPUT_MOUSE_BUTTON_MIDDLE = RJD_INPUT_MOUSE_BUTTON_BEGIN + 2;

void rjd_input_create(struct rjd_input* out, struct rjd_mem_allocator* allocator);
void rjd_input_destroy(struct rjd_input* input);
struct rjd_result rjd_input_hook(struct rjd_input* input, const struct rjd_window* window, const struct rjd_window_environment* env);
void rjd_input_unhook(struct rjd_input* input);

float rjd_input_keyboard_getvalue(enum rjd_input_keyboard code);
float rjd_input_mouse_getvalue(enum rjd_input_mouse code);

// TODO could add support for controllers, joysticks, driving wheels, etc

#if RJD_IMPL && RJD_PLATFORM_WINDOWS

#elif RJD_IMPL && RJD_PLATFORM_OSX

struct rjd_input_osx;

@interface InputResponder : NSResponder
	@property(readonly) BOOL acceptsFirstResponder;
	-(instancetype)initWithInput:(struct rjd_input_osx*)_input;
@end

struct rjd_input_osx
{
	InputResponder* responder;
	const struct rjd_window* window;
};

RJD_STATIC_ASSERT(sizeof(struct rjd_input_osx) <= sizeof(struct rjd_input));

////////////////////////////////////////////////////////////////////////////////
// Interface implementation

void rjd_input_create(struct rjd_input* out, struct rjd_mem_allocator* allocator)
{
	RJD_ASSERT(out);
	RJD_ASSERT(allocator);

	memset(out, 0, sizeof(*out));
}

void rjd_input_destroy(struct rjd_input* input)
{
	RJD_ASSERT(input);

	rjd_input_unhook(input);
	memset(input, 0, sizeof(*input));
}

struct rjd_result rjd_input_hook(struct rjd_input* input, const struct rjd_window* window, const struct rjd_window_environment* env)
{
	RJD_ASSERT(input);
	RJD_ASSERT(env);
	RJD_ASSERT(window);
	RJD_UNUSED_PARAM(env); // used in win32

	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;

	NSWindow* nswindow = rjd_window_osx_get_nswindow(window);
	if (nswindow == nil) {
		return RJD_RESULT("No window available to hook. Did the window initialize correctly?");
	}

	MTKView* view = rjd_window_osx_get_mtkview(window);
	if (view == nil) {
		return RJD_RESULT("No view available in the window to hook. Did the window initialize correctly?");
	}

	input_osx->responder = [[InputResponder alloc] initWithInput:input_osx];
	if (input_osx->responder == nil) {
		return RJD_RESULT("Failed to allocate InputResponder. Are you out of memory?");
	}

	//if ([nswindow makeFirstResponder:input_osx->responder] == NO) {
	//	RJD_LOG("makeFirstResponder failed");
	//	input_osx->responder = nil;
	//	return RJD_RESULT("Unable to hook the NSResponder chain. There is probably a bug in the rjd_window code.");
	//}

	view.nextResponder = input_osx->responder;
	nswindow.nextResponder = input_osx->responder;

	input_osx->window = window;

	// hook our new responder into the responder chain
	//NSResponder* next_in_chain = view.nextResponder;
	//view.nextResponder = input_osx->responder;
	//input_osx->responder.nextResponder = next_in_chain;
	//if ([nswindow makeFirstResponder:input_osx->responder] == NO) {
	//	return RJD_LOG("blah");
	//	return RJD_RESULT("first responder failed to yield");
	//}

	RJD_LOG("all is well");

	return RJD_RESULT_OK();
}

void rjd_input_unhook(struct rjd_input* input)
{
	if (input)
	{
		struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;

		NSWindow* nswindow = rjd_window_osx_get_nswindow(input_osx->window);
		if (nswindow && nswindow.firstResponder == input_osx->responder) {
			[nswindow makeFirstResponder:nswindow];
		}
		input_osx->responder = nil;
		input_osx->window = NULL;
	}
}

float rjd_input_keyboard_getvalue(enum rjd_input_keyboard code)
{
	return 0;
}

float rjd_input_mouse_getvalue(enum rjd_input_mouse code)
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Local helpers

@implementation InputResponder
{
	struct rjd_input_osx* input;
}

-(instancetype)initWithInput:(struct rjd_input_osx*)_input
{
    if (self = [super init]) {
		self->input = _input;
		printf("ok!\n");;
    }
    return self;
}

-(BOOL)acceptsFirstResponder
{
	return YES;
}

-(BOOL)becomeFirstResponder
{
	return YES;
}

-(void)keyDown:(NSEvent*)event
{
	printf("keyDown: %d\n", (int)event.keyCode);
}

-(void)keyUp:(NSEvent*)event
{
	printf("keyUp: %d\n", (int)event.keyCode);
}

-(void)mouseDown:(NSEvent*)event
{
	printf("mouseDown\n");
}

-(void)mouseUp:(NSEvent*)event
{
	printf("mouseUp\n");
}


@end

#endif // RJD_IMPL

