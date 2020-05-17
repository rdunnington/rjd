#pragma once

#define RJD_INPUT_H 1

struct rjd_input
{
	char impl[1024];
};

enum rjd_input_keyboard
{
	RJD_INPUT_KEYBOARD_CONTROL_LEFT,
	RJD_INPUT_KEYBOARD_CONTROL_RIGHT,
	RJD_INPUT_KEYBOARD_SHIFT_LEFT,
	RJD_INPUT_KEYBOARD_SHIFT_RIGHT,
	RJD_INPUT_KEYBOARD_OPTION_LEFT, // OSX only
	RJD_INPUT_KEYBOARD_OPTION_RIGHT, // OSX only
	RJD_INPUT_KEYBOARD_COMMAND_LEFT, // OSX only
	RJD_INPUT_KEYBOARD_COMMAND_RIGHT, // OSX only
	RJD_INPUT_KEYBOARD_ALT_LEFT, // Windows only
	RJD_INPUT_KEYBOARD_ALT_RIGHT, // Windows only
	RJD_INPUT_KEYBOARD_HOME,
	RJD_INPUT_KEYBOARD_END,
	RJD_INPUT_KEYBOARD_DELETE,
	RJD_INPUT_KEYBOARD_PAGEUP,
	RJD_INPUT_KEYBOARD_PAGEDOWN,
    RJD_INPUT_KEYBOARD_RETURN,
	RJD_INPUT_KEYBOARD_ESCAPE,
	RJD_INPUT_KEYBOARD_ARROW_LEFT,
	RJD_INPUT_KEYBOARD_ARROW_RIGHT,
	RJD_INPUT_KEYBOARD_ARROW_UP,
	RJD_INPUT_KEYBOARD_ARROW_DOWN,
	RJD_INPUT_KEYBOARD_NUMPAD_0,
	RJD_INPUT_KEYBOARD_NUMPAD_END = RJD_INPUT_KEYBOARD_NUMPAD_0 + 9,
	RJD_INPUT_KEYBOARD_FUNCTION_0,
	RJD_INPUT_KEYBOARD_FUNCTION_END = RJD_INPUT_KEYBOARD_FUNCTION_0 + 15,
	RJD_INPUT_KEYBOARD_ASCII_BEGIN,
	RJD_INPUT_KEYBOARD_ASCII_END = RJD_INPUT_KEYBOARD_ASCII_BEGIN + 127,
	RJD_INPUT_KEYBOARD_COUNT,
};

enum
{
	RJD_INPUT_MOUSE_MAX_BUTTONS = 16,
};

enum rjd_input_mouse
{
	RJD_INPUT_MOUSE_X,
	RJD_INPUT_MOUSE_Y,
	RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_X,
	RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_Y,
	RJD_INPUT_MOUSE_BUTTON_BEGIN, // Add the index of the desired button to this value
	RJD_INPUT_MOUSE_BUTTON_END = RJD_INPUT_MOUSE_BUTTON_BEGIN + RJD_INPUT_MOUSE_MAX_BUTTONS,
	RJD_INPUT_MOUSE_COUNT,
};

const extern enum rjd_input_mouse RJD_INPUT_MOUSE_BUTTON_LEFT;
const extern enum rjd_input_mouse RJD_INPUT_MOUSE_BUTTON_RIGHT;

// TODO could add support for controllers, joysticks, driving wheels, etc
// TODO debug record/replay

void rjd_input_create(struct rjd_input* out, struct rjd_mem_allocator* allocator);
void rjd_input_destroy(struct rjd_input* input);
struct rjd_result rjd_input_hook(struct rjd_input* input, const struct rjd_window* window, const struct rjd_window_environment* env);
void rjd_input_unhook(struct rjd_input* input);
void rjd_input_markframe(struct rjd_input* input);

bool rjd_input_keyboard_now(const struct rjd_input* input, enum rjd_input_keyboard code);
bool rjd_input_keyboard_prev(const struct rjd_input* input, enum rjd_input_keyboard code);

bool rjd_input_mouse_now(const struct rjd_input* input, enum rjd_input_mouse code);
bool rjd_input_mouse_prev(const struct rjd_input* input, enum rjd_input_mouse code);

static inline bool rjd_input_keyboard_triggered(const struct rjd_input* input, enum rjd_input_keyboard code); // key was pressed starting this frame
static inline bool rjd_input_mouse_triggered(const struct rjd_input* input, enum rjd_input_mouse code); // key was pressed starting this frame

const char* rjd_input_keyboard_tostring(enum rjd_input_keyboard code);

////////////////////////////////////////////////////////////////////////////////
// Inline implementation

static inline bool rjd_input_keyboard_triggered(const struct rjd_input* input, enum rjd_input_keyboard code)
{
	return rjd_input_keyboard_now(input, code) && !rjd_input_keyboard_prev(input, code);
}

static inline bool rjd_input_mouse_triggered(const struct rjd_input* input, enum rjd_input_mouse code)
{
	return rjd_input_mouse_now(input, code) && !rjd_input_mouse_prev(input, code);
}

#if RJD_IMPL

const enum rjd_input_mouse RJD_INPUT_MOUSE_BUTTON_LEFT = RJD_INPUT_MOUSE_BUTTON_BEGIN + 0;
const enum rjd_input_mouse RJD_INPUT_MOUSE_BUTTON_RIGHT = RJD_INPUT_MOUSE_BUTTON_BEGIN + 1;

////////////////////////////////////////////////////////////////////////////////
// Local helpers

#if RJD_PLATFORM_WINDOWS

#elif RJD_PLATFORM_OSX

#include <Carbon/Carbon.h>

////////////////////////////////////////////////////////////////////////////////
// Local helpers

struct rjd_input_osx;

@interface InputResponder : NSResponder
	@property(readonly) BOOL acceptsFirstResponder;
	-(instancetype)initWithInput:(struct rjd_input_osx*)_input;
@end

struct rjd_input_keyboard_state
{
	uint8_t values[RJD_INPUT_KEYBOARD_COUNT];
};

struct rjd_input_mouse_state
{
	float values[RJD_INPUT_MOUSE_MAX_BUTTONS];
};

struct rjd_input_osx
{
	InputResponder* responder;
	const struct rjd_window* window;

	uint8_t now_index;
	struct rjd_input_keyboard_state keyboard[2];
	struct rjd_input_mouse_state mouse[2];
};

RJD_STATIC_ASSERT(sizeof(struct rjd_input_osx) <= sizeof(struct rjd_input));

////////////////////////////////////////////////////////////////////////////////
// Local data

const uint8_t RJD_INPUT_KEYCODE_TO_ENUM[] =
{
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'a',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 's',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'd',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'f',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'h',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'g',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'z',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'x',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'c',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'v',
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'b',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'q',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'w',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'e',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'r',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'y',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 't',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '1',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '2',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '3',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '4',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '6',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '5',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '=',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '9',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '7',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '-',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '8',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '0',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + ']',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'o',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'u',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '[',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'i',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'p',
	RJD_INPUT_KEYBOARD_RETURN,
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'l',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'j',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '\'',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'k',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + ';',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '\\',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + ',',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '/',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'n',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'm',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '.',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '\t',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + ' ',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '~',
	RJD_INPUT_KEYBOARD_DELETE,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_ESCAPE,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT, // command (OSX)
	RJD_INPUT_KEYBOARD_COUNT, // shift (OSX)
	RJD_INPUT_KEYBOARD_COUNT, // option (OSX)
	RJD_INPUT_KEYBOARD_COUNT, // control (OSX)
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT, // numpad .
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT, // numpad *
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT, // numpad +
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT, // numpad /
	RJD_INPUT_KEYBOARD_COUNT, // numpad enter
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT, // numpad -
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT, // numpad =
	RJD_INPUT_KEYBOARD_NUMPAD_0,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 1,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 2,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 3,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 4,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 5,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 6,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 7,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 8,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 9,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 5,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 6,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 7,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 3,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 8,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 9,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 10,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 12,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 4,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 2,
	RJD_INPUT_KEYBOARD_COUNT,
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 1,
	RJD_INPUT_KEYBOARD_ARROW_LEFT,
	RJD_INPUT_KEYBOARD_ARROW_RIGHT,
	RJD_INPUT_KEYBOARD_ARROW_DOWN,
	RJD_INPUT_KEYBOARD_ARROW_UP,
	//RJD_INPUT_KEYBOARD_HOME,
	//RJD_INPUT_KEYBOARD_END,
	//RJD_INPUT_KEYBOARD_PAGEUP,
	//RJD_INPUT_KEYBOARD_PAGEDOWN,
};

const char* RJD_INPUT_KEYBOARD_STRINGS[] =
{
	"RJD_INPUT_KEYBOARD_CONTROL_LEFT",
	"RJD_INPUT_KEYBOARD_CONTROL_RIGHT",
	"RJD_INPUT_KEYBOARD_SHIFT_LEFT",
	"RJD_INPUT_KEYBOARD_SHIFT_RIGHT",
	"RJD_INPUT_KEYBOARD_OPTION_LEFT", // OSX only
	"RJD_INPUT_KEYBOARD_OPTION_RIGHT", // OSX only
	"RJD_INPUT_KEYBOARD_COMMAND_LEFT", // OSX only
	"RJD_INPUT_KEYBOARD_COMMAND_RIGHT", // OSX only
	"RJD_INPUT_KEYBOARD_ALT_LEFT", // Windows only
	"RJD_INPUT_KEYBOARD_ALT_RIGHT", // Windows only
	"RJD_INPUT_KEYBOARD_HOME",
	"RJD_INPUT_KEYBOARD_END",
	"RJD_INPUT_KEYBOARD_DELETE",
	"RJD_INPUT_KEYBOARD_PAGEUP",
	"RJD_INPUT_KEYBOARD_PAGEDOWN",
    "RJD_INPUT_KEYBOARD_RETURN",
	"RJD_INPUT_KEYBOARD_ESCAPE",
	"RJD_INPUT_KEYBOARD_ARROW_LEFT",
	"RJD_INPUT_KEYBOARD_ARROW_RIGHT",
	"RJD_INPUT_KEYBOARD_ARROW_UP",
	"RJD_INPUT_KEYBOARD_ARROW_DOWN",
	"RJD_INPUT_KEYBOARD_NUMPAD_0",
	"RJD_INPUT_KEYBOARD_NUMPAD_1",
	"RJD_INPUT_KEYBOARD_NUMPAD_2",
	"RJD_INPUT_KEYBOARD_NUMPAD_3",
	"RJD_INPUT_KEYBOARD_NUMPAD_4",
	"RJD_INPUT_KEYBOARD_NUMPAD_5",
	"RJD_INPUT_KEYBOARD_NUMPAD_6",
	"RJD_INPUT_KEYBOARD_NUMPAD_7",
	"RJD_INPUT_KEYBOARD_NUMPAD_8",
	"RJD_INPUT_KEYBOARD_NUMPAD_9",
	"RJD_INPUT_KEYBOARD_FUNCTION_0",
	"RJD_INPUT_KEYBOARD_FUNCTION_1",
	"RJD_INPUT_KEYBOARD_FUNCTION_2",
	"RJD_INPUT_KEYBOARD_FUNCTION_3",
	"RJD_INPUT_KEYBOARD_FUNCTION_4",
	"RJD_INPUT_KEYBOARD_FUNCTION_5",
	"RJD_INPUT_KEYBOARD_FUNCTION_6",
	"RJD_INPUT_KEYBOARD_FUNCTION_7",
	"RJD_INPUT_KEYBOARD_FUNCTION_8",
	"RJD_INPUT_KEYBOARD_FUNCTION_9",
	"RJD_INPUT_KEYBOARD_FUNCTION_10",
	"RJD_INPUT_KEYBOARD_FUNCTION_11",
	"RJD_INPUT_KEYBOARD_FUNCTION_12",
	"RJD_INPUT_KEYBOARD_FUNCTION_13",
	"RJD_INPUT_KEYBOARD_FUNCTION_14",
	"RJD_INPUT_KEYBOARD_FUNCTION_15",
	"NUL",
	"SOH",
	"STX",
	"ETX",
	"EOT",
	"ENQ",
	"ACK",
	"BEL",
	"BS",
	"HT",
	"LF",
	"VT",
	"FF",
	"CR",
	"SO",
	"SI",
	"DLE",
	"DC1",
	"DC2",
	"DC3",
	"DC4",
	"NAK",
	"SYN",
	"ETB",
	"CAN",
	"EM",
	"SUB",
	"ESC",
	"FS",
	"GS",
	"RS",
	"US",
	"space",
	"!",
	",",
	"#",
	"$",
	"%",
	"&",
	"'",
	"(",
	")",
	"*",
	"+",
	",",
	"-",
	".",
	"/",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	":",
	";",
	"<",
	"=",
	">",
	"?",
	"@",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"[",
	"\\",
	"]",
	"^",
	"_",
	"`",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z",
	"{",
	"|",
	"}",
	"~",
	"DEL",
};

RJD_STATIC_ASSERT(rjd_countof(RJD_INPUT_KEYBOARD_STRINGS) == RJD_INPUT_KEYBOARD_COUNT);

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

	view.nextResponder = input_osx->responder;
	nswindow.acceptsMouseMovedEvents = YES;
	nswindow.nextResponder = input_osx->responder;

	input_osx->window = window;

	return RJD_RESULT_OK();
}

void rjd_input_unhook(struct rjd_input* input)
{
	RJD_ASSERT(input);
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;

	NSWindow* nswindow = rjd_window_osx_get_nswindow(input_osx->window);
	if (nswindow && nswindow.firstResponder == input_osx->responder) {
		[nswindow makeFirstResponder:nswindow];
	}
	input_osx->responder = nil;
	input_osx->window = NULL;
}

void rjd_input_markframe(struct rjd_input* input)
{
	RJD_ASSERT(input);
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;

	uint32_t prev_index = input_osx->now_index;
	input_osx->now_index = (input_osx->now_index + 1) % 2;

	memcpy(input_osx->keyboard + input_osx->now_index, input_osx->keyboard + prev_index, sizeof(*input_osx->keyboard));
	memcpy(input_osx->mouse + input_osx->now_index, input_osx->mouse + prev_index, sizeof(*input_osx->mouse));

	struct rjd_input_mouse_state* mouse_now = input_osx->mouse + input_osx->now_index;
	mouse_now->values[RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_X] = 0;
	mouse_now->values[RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_Y] = 0;
}

bool rjd_input_keyboard_now(const struct rjd_input* input, enum rjd_input_keyboard code)
{
	RJD_ASSERT(input);
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;

	struct rjd_input_keyboard_state* state = input_osx->keyboard + input_osx->now_index;
	RJD_ASSERT(code < rjd_countof(state->values));
	bool value = state->values[code];
	return value;
}

bool rjd_input_keyboard_prev(const struct rjd_input* input, enum rjd_input_keyboard code)
{
	RJD_ASSERT(input);
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;

	uint32_t prev_index = (input_osx->now_index + 1) % 2;
	struct rjd_input_keyboard_state* state = input_osx->keyboard + prev_index;
	RJD_ASSERT(code < rjd_countof(state->values));
	bool value = state->values[code];
	return value;
}

bool rjd_input_mouse_now(const struct rjd_input* input, enum rjd_input_mouse code)
{
	RJD_ASSERT(input);
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;
	struct rjd_input_mouse_state* state = input_osx->mouse + input_osx->now_index;
	RJD_ASSERT(code < rjd_countof(state->values));
	float value = state->values[code];
	return value;
}

bool rjd_input_mouse_prev(const struct rjd_input* input, enum rjd_input_mouse code)
{
	RJD_ASSERT(input);
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;
	uint32_t prev_index = (input_osx->now_index + 1) % 2;
	struct rjd_input_mouse_state* state = input_osx->mouse + prev_index;
	RJD_ASSERT(code < rjd_countof(state->values));
	float value = state->values[code];
	return value;
}

const char* rjd_input_keyboard_tostring(enum rjd_input_keyboard code)
{
	if (code < rjd_countof(RJD_INPUT_KEYBOARD_STRINGS)) {
		return RJD_INPUT_KEYBOARD_STRINGS[code];
	}
	return NULL;
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
	if (event.keyCode < rjd_countof(RJD_INPUT_KEYCODE_TO_ENUM)) {
		enum rjd_input_keyboard code = RJD_INPUT_KEYCODE_TO_ENUM[event.keyCode];
		input->keyboard[input->now_index].values[code] = 1;
	}
}

-(void)keyUp:(NSEvent*)event
{
	if (event.keyCode < rjd_countof(RJD_INPUT_KEYCODE_TO_ENUM)) {
		enum rjd_input_keyboard code = RJD_INPUT_KEYCODE_TO_ENUM[event.keyCode];
		input->keyboard[input->now_index].values[code] = 0;
	}
}

-(void)flagsChanged:(NSEvent*)event
{
	struct rjd_input_keyboard_state* state = input->keyboard + input->now_index;

	state->values[RJD_INPUT_KEYBOARD_SHIFT_LEFT] = (event.modifierFlags & NX_DEVICELSHIFTKEYMASK) ? 1 : 0;
	state->values[RJD_INPUT_KEYBOARD_SHIFT_RIGHT] = (event.modifierFlags & NX_DEVICERSHIFTKEYMASK) ? 1 : 0;
	state->values[RJD_INPUT_KEYBOARD_CONTROL_LEFT] = (event.modifierFlags & NX_DEVICELCTLKEYMASK) ? 1 : 0;
	state->values[RJD_INPUT_KEYBOARD_CONTROL_RIGHT] = (event.modifierFlags & NX_DEVICERCTLKEYMASK) ? 1 : 0;
	state->values[RJD_INPUT_KEYBOARD_OPTION_LEFT] = (event.modifierFlags & NX_DEVICELALTKEYMASK) ? 1 : 0;
	state->values[RJD_INPUT_KEYBOARD_OPTION_RIGHT] = (event.modifierFlags & NX_DEVICERALTKEYMASK) ? 1 : 0;
	state->values[RJD_INPUT_KEYBOARD_COMMAND_LEFT] = (event.modifierFlags & NX_DEVICELCMDKEYMASK) ? 1 : 0;
	state->values[RJD_INPUT_KEYBOARD_COMMAND_RIGHT] = (event.modifierFlags & NX_DEVICERCMDKEYMASK) ? 1 : 0;
}

-(void)mouseDown:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
    input->mouse[input->now_index].values[RJD_INPUT_MOUSE_BUTTON_LEFT] = 1;
}

-(void)mouseUp:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
    input->mouse[input->now_index].values[RJD_INPUT_MOUSE_BUTTON_LEFT] = 0;
}
                  
-(void)rightMouseDown:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
	input->mouse[input->now_index].values[RJD_INPUT_MOUSE_BUTTON_RIGHT] = 1;
}

-(void)rightMouseUp:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
	input->mouse[input->now_index].values[RJD_INPUT_MOUSE_BUTTON_RIGHT] = 0;
}
                  
-(void)otherMouseDown:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
	for (uint32_t button = RJD_INPUT_MOUSE_BUTTON_BEGIN; button < RJD_INPUT_MOUSE_BUTTON_END; ++button) {
		uint32_t bit = 1 << (button - RJD_INPUT_MOUSE_BUTTON_BEGIN);
		bool pressed = (NSEvent.pressedMouseButtons & bit) != 0;
		input->mouse[input->now_index].values[button] = pressed;
	}
}

-(void)otherMouseUp:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
	for (uint32_t button = RJD_INPUT_MOUSE_BUTTON_BEGIN; button < RJD_INPUT_MOUSE_BUTTON_END; ++button) {
		uint32_t bit = 1 << (button - RJD_INPUT_MOUSE_BUTTON_BEGIN);
		bool pressed = (NSEvent.pressedMouseButtons & bit) != 0;
		input->mouse[input->now_index].values[button] = pressed;
	}
}

-(void)mouseMoved:(NSEvent*)event
{
	MTKView* mtkview = rjd_window_osx_get_mtkview(input->window);
	NSPoint locationInView = [mtkview convertPoint:event.locationInWindow fromView:nil];

	int x = locationInView.x;
	int y = locationInView.y;

	struct rjd_window_size size = rjd_window_size_get(input->window);
	if (x >= 0 && x <= size.width && y >= 0 && y <= size.height) {
		input->mouse[input->now_index].values[RJD_INPUT_MOUSE_X] = (float)x;
		input->mouse[input->now_index].values[RJD_INPUT_MOUSE_X] = (float)y;
	}
}

-(void)scrollWheel:(NSEvent*)event
{
	input->mouse[input->now_index].values[RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_X] = (float)event.scrollingDeltaX;
	input->mouse[input->now_index].values[RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_Y] = (float)event.scrollingDeltaY;
}

@end

#endif // RJD_PLATFORM_*
#endif // RJD_IMPL
