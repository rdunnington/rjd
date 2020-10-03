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

	// Convenience values
	RJD_INPUT_MOUSE_BUTTON_LEFT = RJD_INPUT_MOUSE_BUTTON_BEGIN,
	RJD_INPUT_MOUSE_BUTTON_RIGHT
};

enum rjd_input_sim_type
{
	RJD_INPUT_SIM_TYPE_KEYBOARD,
	RJD_INPUT_SIM_TYPE_MOUSE,
};

struct rjd_input_sim_event
{
	enum rjd_input_sim_type type;
	union
	{
		struct
		{
			enum rjd_input_keyboard key;
			bool is_down;
		} keyboard;

		struct 
		{
			enum rjd_input_mouse button;
			float value;
		} mouse;
	};
};

// TODO could add support for controllers, joysticks, driving wheels, etc
// TODO debug record/replay

void rjd_input_create(struct rjd_input* out, struct rjd_mem_allocator* allocator);
void rjd_input_destroy(struct rjd_input* input);
struct rjd_result rjd_input_hook(struct rjd_input* input, const struct rjd_window* window, const struct rjd_window_environment* env);
void rjd_input_unhook(struct rjd_input* input);
void rjd_input_markframe(struct rjd_input* input);

bool rjd_input_keyboard_now(const struct rjd_input* input, enum rjd_input_keyboard code);
bool rjd_input_keyboard_prev(const struct rjd_input* input, enum rjd_input_keyboard code);

float rjd_input_mouse_now(const struct rjd_input* input, enum rjd_input_mouse code);
float rjd_input_mouse_prev(const struct rjd_input* input, enum rjd_input_mouse code);

static inline bool rjd_input_keyboard_triggered(const struct rjd_input* input, enum rjd_input_keyboard code); // key was pressed starting this frame
static inline bool rjd_input_mouse_triggered(const struct rjd_input* input, enum rjd_input_mouse code); // button was pressed starting this frame

const char* rjd_input_keyboard_tostring(enum rjd_input_keyboard code);

void rjd_input_simulate(struct rjd_input* input, struct rjd_input_sim_event event);

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

////////////////////////////////////////////////////////////////////////////////
// platform-independent impl

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

const char* rjd_input_keyboard_tostring(enum rjd_input_keyboard code)
{
	if (code < rjd_countof(RJD_INPUT_KEYBOARD_STRINGS)) {
		return RJD_INPUT_KEYBOARD_STRINGS[code];
	}
	return NULL;
}

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

float rjd_input_mouse_now(const struct rjd_input* input, enum rjd_input_mouse code)
{
	RJD_ASSERT(input);

	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;
	struct rjd_input_mouse_state* state = input_osx->mouse + input_osx->now_index;
	RJD_ASSERT(code < rjd_countof(state->values));
	float value = state->values[code];
	return value;
}

float rjd_input_mouse_prev(const struct rjd_input* input, enum rjd_input_mouse code)
{
	RJD_ASSERT(input);

	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;
	uint32_t prev_index = (input_osx->now_index + 1) % 2;
	struct rjd_input_mouse_state* state = input_osx->mouse + prev_index;
	RJD_ASSERT(code < rjd_countof(state->values));
	float value = state->values[code];
	return value;
}

void rjd_input_simulate(struct rjd_input* input, struct rjd_input_sim_event event)
{
	RJD_ASSERT(input);

	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;

	switch (event.type)
	{
		case RJD_INPUT_SIM_TYPE_KEYBOARD:
		{
            // TODO take into account is_down flag AND make sure modifier_flag reflects the entire state of the modifier flags!
            NSEventModifierFlags modifier_flag = 0;
			modifier_flag |= rjd_input_keyboard_now(input, RJD_INPUT_KEYBOARD_SHIFT_LEFT)   	? NX_DEVICELSHIFTKEYMASK : 0;
			modifier_flag |= rjd_input_keyboard_now(input, RJD_INPUT_KEYBOARD_SHIFT_RIGHT)		? NX_DEVICERSHIFTKEYMASK : 0;
			modifier_flag |= rjd_input_keyboard_now(input, RJD_INPUT_KEYBOARD_CONTROL_LEFT) 	? NX_DEVICELCTLKEYMASK : 0;
			modifier_flag |= rjd_input_keyboard_now(input, RJD_INPUT_KEYBOARD_CONTROL_RIGHT)	? NX_DEVICERCTLKEYMASK : 0;
			modifier_flag |= rjd_input_keyboard_now(input, RJD_INPUT_KEYBOARD_OPTION_LEFT)		? NX_DEVICELALTKEYMASK : 0;
			modifier_flag |= rjd_input_keyboard_now(input, RJD_INPUT_KEYBOARD_OPTION_RIGHT)		? NX_DEVICERALTKEYMASK : 0;
			modifier_flag |= rjd_input_keyboard_now(input, RJD_INPUT_KEYBOARD_COMMAND_LEFT)		? NX_DEVICELCMDKEYMASK : 0;
			modifier_flag |= rjd_input_keyboard_now(input, RJD_INPUT_KEYBOARD_COMMAND_RIGHT)	? NX_DEVICERCMDKEYMASK : 0;

			NSEventModifierFlags key_bit = 0;
            switch (event.keyboard.key)
            {
                case RJD_INPUT_KEYBOARD_SHIFT_LEFT:     key_bit = NX_DEVICELSHIFTKEYMASK; break;
                case RJD_INPUT_KEYBOARD_SHIFT_RIGHT:    key_bit = NX_DEVICERSHIFTKEYMASK; break;
                case RJD_INPUT_KEYBOARD_CONTROL_LEFT:   key_bit = NX_DEVICELCTLKEYMASK; break;
                case RJD_INPUT_KEYBOARD_CONTROL_RIGHT:  key_bit = NX_DEVICERCTLKEYMASK; break;
                case RJD_INPUT_KEYBOARD_OPTION_LEFT:    key_bit = NX_DEVICELALTKEYMASK; break;
                case RJD_INPUT_KEYBOARD_OPTION_RIGHT:   key_bit = NX_DEVICERALTKEYMASK; break;
                case RJD_INPUT_KEYBOARD_COMMAND_LEFT:   key_bit = NX_DEVICELCMDKEYMASK; break;
                case RJD_INPUT_KEYBOARD_COMMAND_RIGHT:  key_bit = NX_DEVICERCMDKEYMASK; break;
                default: break;
            }

            if (key_bit) {
				if (event.keyboard.is_down) {
					modifier_flag |= key_bit;
				} else {
					modifier_flag &= ~(key_bit);
				}

				NSEvent* nsevent = [NSEvent keyEventWithType:NSEventTypeKeyDown
					location:NSZeroPoint
					modifierFlags:modifier_flag
					timestamp:[NSDate date].timeIntervalSince1970
					windowNumber:0
					context:nil
					characters:@""
					charactersIgnoringModifiers:@""
					isARepeat:false
					keyCode:0];
                  
                [input_osx->responder flagsChanged:nsevent];
            } else {
                int32_t keycode = -1;
                for (int32_t i = 0; i < (int32_t)rjd_countof(RJD_INPUT_KEYCODE_TO_ENUM); ++i) {
                    if (RJD_INPUT_KEYCODE_TO_ENUM[i] == event.keyboard.key) {
                        keycode = i;
                        break;
                    }
                }
                if (keycode < 0) {
                    RJD_ASSERT(keycode >= 0);
                    return;
                }

                NSEvent* nsevent = [NSEvent keyEventWithType:event.keyboard.is_down ? NSEventTypeKeyDown : NSEventTypeKeyUp
					location:NSZeroPoint
					modifierFlags:0
					timestamp:[NSDate date].timeIntervalSince1970
					windowNumber:0
					context:nil
					characters:@""
					charactersIgnoringModifiers:@""
					isARepeat:false
					keyCode:keycode];

                if (event.keyboard.is_down) {
                    [input_osx->responder keyDown:nsevent];
                } else {
                    [input_osx->responder keyUp:nsevent];
                }
            }
			break;
		}

		case RJD_INPUT_SIM_TYPE_MOUSE:
		{
			switch (event.mouse.button)
			{
				case RJD_INPUT_MOUSE_X:
				case RJD_INPUT_MOUSE_Y:
				{
					const MTKView* mtkview = rjd_window_osx_get_mtkview(input_osx->window);
					const NSPoint origin_in_window = {0,0};
					const NSPoint origin_in_view = [mtkview convertPoint:origin_in_window fromView:nil];

					NSPoint xy = {
						.x = rjd_input_mouse_now(input, RJD_INPUT_MOUSE_X),
						.y = rjd_input_mouse_now(input, RJD_INPUT_MOUSE_Y),
					};

					if (event.mouse.button == RJD_INPUT_MOUSE_X) {
						xy.x = event.mouse.value;
					} else {
						xy.y = event.mouse.value;
					}

					xy.x -= origin_in_view.x;
					xy.y -= origin_in_view.y;

					NSEvent* nsevent = [NSEvent mouseEventWithType:NSEventTypeMouseMoved
                       location:xy
                  modifierFlags:0
                      timestamp:[NSDate date].timeIntervalSince1970
                   windowNumber:0
                        context:nil
                    eventNumber:0
                     clickCount:0
                       pressure:0];

					[input_osx->responder mouseMoved:nsevent];
					break;
				}

				case RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_X:
				case RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_Y:
				{
					CGWheelCount count_wheels = 2; // range in [1,3] where axes are [Y,X,Z]
					int32_t dx = 0;
					int32_t dy = 0;
					if (event.mouse.button == RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_X) {
						dx = event.mouse.value;
					} else {
						dy = event.mouse.value;
					}

					CGEventRef cgevent = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitLine, count_wheels, dy, dx);
					NSEvent* nsevent = [NSEvent eventWithCGEvent:cgevent];
					[input_osx->responder scrollWheel:nsevent];
                    break;
				}

				case RJD_INPUT_MOUSE_BUTTON_LEFT:
                {
					if (event.mouse.value) {
						[input_osx->responder mouseDown:[[NSEvent alloc] init]];
					} else {
						[input_osx->responder mouseUp:[[NSEvent alloc] init]];
					}
					break;
                }

				case RJD_INPUT_MOUSE_BUTTON_RIGHT:
                {
					if (event.mouse.value) {
						[input_osx->responder rightMouseDown:[[NSEvent alloc] init]];
					} else {
						[input_osx->responder rightMouseUp:[[NSEvent alloc] init]];
					}
					break;
                }

				default:
					// The rest of the events are intentionally left unhandled
					break;
			}
			break;
		}
	}
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
		input->mouse[input->now_index].values[RJD_INPUT_MOUSE_Y] = (float)y;
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
