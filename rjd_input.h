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
	RJD_INPUT_KEYBOARD_BACKSPACE, // TODO OSX
	RJD_INPUT_KEYBOARD_PAGEUP,
	RJD_INPUT_KEYBOARD_PAGEDOWN,
    RJD_INPUT_KEYBOARD_RETURN,
	RJD_INPUT_KEYBOARD_ESCAPE,
	RJD_INPUT_KEYBOARD_PAUSE, // TODO OSX
	RJD_INPUT_KEYBOARD_CAPSLOCK, // TODO OSX
	RJD_INPUT_KEYBOARD_PRINTSCREEN, // TODO OSX
	RJD_INPUT_KEYBOARD_INSERT, // TODO OSX
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

void rjd_input_simulate(struct rjd_input* input, struct rjd_input_sim_event event);

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

////////////////////////////////////////////////////////////////////////////////
// platform-independent impl

struct rjd_input_keyboard_state
{
	uint8_t values[RJD_INPUT_KEYBOARD_COUNT];
};

struct rjd_input_mouse_state
{
	float values[RJD_INPUT_MOUSE_MAX_BUTTONS];
};

struct rjd_input_common
{
	struct rjd_input_keyboard_state keyboard[2];
	struct rjd_input_mouse_state mouse[2];
	uint8_t now_index;
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
	"RJD_INPUT_KEYBOARD_BACKSPACE",
	"RJD_INPUT_KEYBOARD_PAGEUP",
	"RJD_INPUT_KEYBOARD_PAGEDOWN",
    "RJD_INPUT_KEYBOARD_RETURN",
	"RJD_INPUT_KEYBOARD_ESCAPE",
	"RJD_INPUT_KEYBOARD_PAUSE",
	"RJD_INPUT_KEYBOARD_CAPSLOCK",
	"RJD_INPUT_KEYBOARD_PRINTSCREEN",
	"RJD_INPUT_KEYBOARD_INSERT",
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

void rjd_input_markframe_common(struct rjd_input_common* input)
{
	RJD_ASSERT(input);

	uint32_t prev_index = input->now_index;
	input->now_index = (input->now_index + 1) % 2;

	memcpy(input->keyboard + input->now_index, input->keyboard + prev_index, sizeof(*input->keyboard));
	memcpy(input->mouse + input->now_index, input->mouse + prev_index, sizeof(*input->mouse));

	struct rjd_input_mouse_state* mouse_now = input->mouse + input->now_index;
	mouse_now->values[RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_X] = 0;
	mouse_now->values[RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_Y] = 0;
}

bool rjd_input_keyboard_now_common(const struct rjd_input_common* input, enum rjd_input_keyboard code)
{
	RJD_ASSERT(input);

	const struct rjd_input_keyboard_state* state = input->keyboard + input->now_index;
	RJD_ASSERT(code < rjd_countof(state->values));
	bool value = state->values[code];
	return value;
}

bool rjd_input_keyboard_prev_common(const struct rjd_input_common* input, enum rjd_input_keyboard code)
{
	RJD_ASSERT(input);

	uint32_t prev_index = (input->now_index + 1) % 2;
	const struct rjd_input_keyboard_state* state = input->keyboard + prev_index;
	RJD_ASSERT(code < rjd_countof(state->values));
	bool value = state->values[code];
	return value;
}

float rjd_input_mouse_now_common(const struct rjd_input_common* input, enum rjd_input_mouse code)
{
	RJD_ASSERT(input);

	const struct rjd_input_mouse_state* state = input->mouse + input->now_index;
	RJD_ASSERT(code < rjd_countof(state->values));
	float value = state->values[code];
	return value;
}

float rjd_input_mouse_prev_common(const struct rjd_input_common* input, enum rjd_input_mouse code)
{
	RJD_ASSERT(input);

	uint32_t prev_index = (input->now_index + 1) % 2;
	const struct rjd_input_mouse_state* state = input->mouse + prev_index;
	RJD_ASSERT(code < rjd_countof(state->values));
	float value = state->values[code];
	return value;
}

#if RJD_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h> // GET_X_LPARAM, GET_Y_LPARAM

struct rjd_input_win32
{
	const struct rjd_window* window;

	HHOOK hook_handle_wndproc;
	HHOOK hook_handle_getmessage;

	struct rjd_input_common common;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_input_win32) <= sizeof(struct rjd_input));

enum
{
	RJD_INPUT_WIN32_WINDOW_PTR_INDEX = GWLP_USERDATA,
};

LRESULT CALLBACK rjd_input_wndproc_hook(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK rjd_input_getmessage_hook(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT rjd_input_hook_common(struct rjd_input_win32* input_win32, UINT message, WPARAM wParam, LPARAM lParam, int nCode, WPARAM hook_wparam, LPARAM hook_lparam);

const enum rjd_input_keyboard RJD_INPUT_WIN32_KEYCODE_TO_ENUM[] =
{
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x00 invalid
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x01 VK_LBUTTON
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x02 VK_RBUTTON
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x03 VK_CANCEL
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x04 VK_MBUTTON
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x05 VK_XBUTTON1
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x06 VK_XBUTTON2
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x07 reserved
	RJD_INPUT_KEYBOARD_BACKSPACE,			// 0x08 VK_BACK
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '	',	// 0x09 VK_TAB
	RJD_INPUT_KEYBOARD_COUNT,				// 0x0A undefined
	RJD_INPUT_KEYBOARD_COUNT,				// 0x0B undefined
	RJD_INPUT_KEYBOARD_COUNT,				// 0x0C VK_CLEAR
	RJD_INPUT_KEYBOARD_RETURN,				// 0x0D VK_RETURN
	RJD_INPUT_KEYBOARD_COUNT,				// 0x0E undefined
	RJD_INPUT_KEYBOARD_COUNT,				// 0x0F undefined
	RJD_INPUT_KEYBOARD_COUNT,				// 0x10 VK_SHIFT (but we use L and R versions)
	RJD_INPUT_KEYBOARD_COUNT,				// 0x11 VK_CONTROL (but we use L and R versions)
	RJD_INPUT_KEYBOARD_COUNT,				// 0x12 VK_MENU (alt, but we use L and R versions)
	RJD_INPUT_KEYBOARD_PAUSE,				// 0x13 VK_PAUSE
	RJD_INPUT_KEYBOARD_CAPSLOCK,			// 0x14 VK_CAPITAL
	RJD_INPUT_KEYBOARD_COUNT,				// 0x15 VK_KANA/VK_HANGEUL/VK_HANGUL
	RJD_INPUT_KEYBOARD_COUNT,				// 0x16 undefined
	RJD_INPUT_KEYBOARD_COUNT,				// 0x17 VK_JUNJA
	RJD_INPUT_KEYBOARD_COUNT,				// 0x18 VK_FINAL
	RJD_INPUT_KEYBOARD_COUNT,				// 0x19 VK_KANJI
	RJD_INPUT_KEYBOARD_COUNT,				// 0x1A undefined
	RJD_INPUT_KEYBOARD_ESCAPE,				// 0x1B VK_ESCAPE
	RJD_INPUT_KEYBOARD_COUNT,				// 0x1C VK_CONVERT
	RJD_INPUT_KEYBOARD_COUNT,				// 0x1D VK_NONCONVERT
	RJD_INPUT_KEYBOARD_COUNT,				// 0x1E VK_ACCEPT
	RJD_INPUT_KEYBOARD_COUNT,				// 0x1F VK_MODECHANGE
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + ' ',	// 0x20 VK_SPACE
	RJD_INPUT_KEYBOARD_PAGEUP,				// 0x21 VK_PRIOR
	RJD_INPUT_KEYBOARD_PAGEDOWN,			// 0x22 VK_NEXT
	RJD_INPUT_KEYBOARD_END,					// 0x23 VK_END
	RJD_INPUT_KEYBOARD_HOME,				// 0x24 VK_HOME
	RJD_INPUT_KEYBOARD_ARROW_LEFT,			// 0x25 VK_LEFT
	RJD_INPUT_KEYBOARD_ARROW_UP,			// 0x26 VK_UP
	RJD_INPUT_KEYBOARD_ARROW_RIGHT,			// 0x27 VK_RIGHT
	RJD_INPUT_KEYBOARD_ARROW_DOWN,			// 0x28 VK_DOWN
	RJD_INPUT_KEYBOARD_COUNT,				// 0x29 VK_SELECT
	RJD_INPUT_KEYBOARD_COUNT,				// 0x2A VK_PRINT
	RJD_INPUT_KEYBOARD_COUNT,				// 0x2B VK_EXECUTE
	RJD_INPUT_KEYBOARD_PRINTSCREEN,			// 0x2C VK_SNAPSHOT
	RJD_INPUT_KEYBOARD_INSERT,				// 0x2D VK_INSERT
	RJD_INPUT_KEYBOARD_DELETE,				// 0x2E VK_DELETE
	RJD_INPUT_KEYBOARD_COUNT,				// 0x2F VK_HELP
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '0',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '1',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '2',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '3',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '4',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '5',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '6',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '7',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '8',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '9',
	RJD_INPUT_KEYBOARD_COUNT,				// 0x3A
	RJD_INPUT_KEYBOARD_COUNT,				// 0x3B
	RJD_INPUT_KEYBOARD_COUNT,				// 0x3C
	RJD_INPUT_KEYBOARD_COUNT,				// 0x3D
	RJD_INPUT_KEYBOARD_COUNT,				// 0x3E
	RJD_INPUT_KEYBOARD_COUNT,				// 0x3F
	RJD_INPUT_KEYBOARD_COUNT,				// 0x40
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'a',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'b',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'c',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'd',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'e',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'f',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'g',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'h',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'i',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'j',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'k',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'l',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'm',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'n',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'o',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'p',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'q',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'r',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 's',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 't',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'u',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'v',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'w',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'x',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'y',
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + 'z',
	RJD_INPUT_KEYBOARD_COUNT,				// 0x5B VK_LWIN
	RJD_INPUT_KEYBOARD_COUNT,				// 0x5C VK_RWIN
	RJD_INPUT_KEYBOARD_COUNT,				// 0x5D VK_APPS
	RJD_INPUT_KEYBOARD_COUNT,				// 0x5E reserved
	RJD_INPUT_KEYBOARD_COUNT,				// 0x5F VK_SLEEP
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 0,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 1,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 2,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 3,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 4,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 5,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 6,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 7,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 8,
	RJD_INPUT_KEYBOARD_NUMPAD_0 + 9,
	RJD_INPUT_KEYBOARD_COUNT,				// 0x6A VK_MULTIPLY
	RJD_INPUT_KEYBOARD_COUNT,				// 0x6B VK_ADD
	RJD_INPUT_KEYBOARD_COUNT,				// 0x6C VK_SEPARATOR
	RJD_INPUT_KEYBOARD_COUNT,				// 0x6D VK_SUBTRACT
	RJD_INPUT_KEYBOARD_COUNT,				// 0x6E VK_DECIMAL
	RJD_INPUT_KEYBOARD_COUNT,				// 0x6F VK_DIVIDE
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 1,		// 0x70 VK_F1
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 2,		// 0x71 VK_F2
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 3,		// 0x72 VK_F3
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 4,		// 0x73 VK_F4
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 5,		// 0x74 VK_F5
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 6,		// 0x75 VK_F6
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 7,		// 0x76 VK_F7
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 8,		// 0x77 VK_F8
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 9,		// 0x78 VK_F9
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 10,		// 0x79 VK_F10
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 11,		// 0x7A VK_F11
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 12,		// 0x7B VK_F12
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 13,		// 0x7C VK_F13
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 14,		// 0x7D VK_F14
	RJD_INPUT_KEYBOARD_FUNCTION_0 + 15,		// 0x7E VK_F15

	RJD_INPUT_KEYBOARD_COUNT,				// 0x88 VK_NAVIGATION_VIEW
	RJD_INPUT_KEYBOARD_COUNT,				// 0x89 VK_NAVIGATION_MENU
	RJD_INPUT_KEYBOARD_COUNT,				// 0x8A VK_NAVIGATION_UP
	RJD_INPUT_KEYBOARD_COUNT,				// 0x8B VK_NAVIGATION_DOWN
	RJD_INPUT_KEYBOARD_COUNT,				// 0x8C VK_NAVIGATION_LEFT
	RJD_INPUT_KEYBOARD_COUNT,				// 0x8D VK_NAVIGATION_RIGHT
	RJD_INPUT_KEYBOARD_COUNT,				// 0x8E VK_NAVIGATION_ACCEPT
	RJD_INPUT_KEYBOARD_COUNT,				// 0x8F VK_NAVIGATION_CANCEL
	RJD_INPUT_KEYBOARD_COUNT,				// 0x90 VK_NUMLOCK
	RJD_INPUT_KEYBOARD_COUNT,				// 0x91 VK_SCROLL
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '=',	// 0x92 VK_OEM_NEC_EQUAL
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x92 VK_OEM_FJ_JISHO
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x93 VK_OEM_FJ_MASSHOU
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x94 VK_OEM_FJ_TOUROKU
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x95 VK_OEM_FJ_LOYA
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x96 VK_OEM_FJ_ROYA
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x97 reserved          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x98 reserved          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x99 reserved          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x9A reserved          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x9B reserved          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x9C reserved          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x9D reserved          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x9E reserved          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0x9F reserved          
	RJD_INPUT_KEYBOARD_SHIFT_LEFT, 			// 0xA0 VK_LSHIFT         
	RJD_INPUT_KEYBOARD_SHIFT_RIGHT, 		// 0xA1 VK_RSHIFT         
	RJD_INPUT_KEYBOARD_CONTROL_LEFT, 		// 0xA2 VK_LCONTROL       
	RJD_INPUT_KEYBOARD_CONTROL_RIGHT, 		// 0xA3 VK_RCONTROL       
	RJD_INPUT_KEYBOARD_ALT_LEFT, 			// 0xA4 VK_LMENU          
	RJD_INPUT_KEYBOARD_ALT_RIGHT, 			// 0xA5 VK_RMENU          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xA6 VK_BROWSER_BACK        
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xA7 VK_BROWSER_FORWARD     
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xA8 VK_BROWSER_REFRESH     
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xA9 VK_BROWSER_STOP        
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xAA VK_BROWSER_SEARCH      
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xAB VK_BROWSER_FAVORITES   
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xAC VK_BROWSER_HOME        
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xAD VK_VOLUME_MUTE         
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xAE VK_VOLUME_DOWN         
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xAF VK_VOLUME_UP           
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xB0 VK_MEDIA_NEXT_TRACK    
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xB1 VK_MEDIA_PREV_TRACK    
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xB2 VK_MEDIA_STOP          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xB3 VK_MEDIA_PLAY_PAUSE    
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xB4 VK_LAUNCH_MAIL         
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xB5 VK_LAUNCH_MEDIA_SELECT 
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xB6 VK_LAUNCH_APP1         
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xB7 VK_LAUNCH_APP2         
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xB8 reserved          
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xB9 reserved          
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + ';',	// 0xBA VK_OEM_1        ';:' for US
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '=',	// 0xBB VK_OEM_PLUS     '+' any country
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + ',',	// 0xBC VK_OEM_COMMA    ',' any country
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '-',	// 0xBD VK_OEM_MINUS    '-' any country
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '.',	// 0xBE VK_OEM_PERIOD   '.' any country
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '/',	// 0xBF VK_OEM_2        '/?' for US
	RJD_INPUT_KEYBOARD_COUNT,				// 0xC0 VK_OEM_3  '`~' for US
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + ';',	// 0xC1 reserved          
	RJD_INPUT_KEYBOARD_COUNT,				// 0xC2 reserved          
	RJD_INPUT_KEYBOARD_COUNT,				// 0xC3 VK_GAMEPAD_A
	RJD_INPUT_KEYBOARD_COUNT,				// 0xC4 VK_GAMEPAD_B
	RJD_INPUT_KEYBOARD_COUNT,				// 0xC5 VK_GAMEPAD_X
	RJD_INPUT_KEYBOARD_COUNT,				// 0xC6 VK_GAMEPAD_Y
	RJD_INPUT_KEYBOARD_COUNT,				// 0xC7 VK_GAMEPAD_RIGHT_SHOULDER
	RJD_INPUT_KEYBOARD_COUNT,				// 0xC8 VK_GAMEPAD_LEFT_SHOULDER
	RJD_INPUT_KEYBOARD_COUNT,				// 0xC9 VK_GAMEPAD_LEFT_TRIGGER
	RJD_INPUT_KEYBOARD_COUNT,				// 0xCA VK_GAMEPAD_RIGHT_TRIGGER
	RJD_INPUT_KEYBOARD_COUNT,				// 0xCB VK_GAMEPAD_DPAD_UP
	RJD_INPUT_KEYBOARD_COUNT,				// 0xCC VK_GAMEPAD_DPAD_DOWN
	RJD_INPUT_KEYBOARD_COUNT,				// 0xCD VK_GAMEPAD_DPAD_LEFT
	RJD_INPUT_KEYBOARD_COUNT,				// 0xCE VK_GAMEPAD_DPAD_RIGHT
	RJD_INPUT_KEYBOARD_COUNT,				// 0xCF VK_GAMEPAD_MENU
	RJD_INPUT_KEYBOARD_COUNT,				// 0xD0 VK_GAMEPAD_VIEW
	RJD_INPUT_KEYBOARD_COUNT,				// 0xD1 VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON
	RJD_INPUT_KEYBOARD_COUNT,				// 0xD2 VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON
	RJD_INPUT_KEYBOARD_COUNT,				// 0xD3 VK_GAMEPAD_LEFT_THUMBSTICK_UP
	RJD_INPUT_KEYBOARD_COUNT,				// 0xD4 VK_GAMEPAD_LEFT_THUMBSTICK_DOWN
	RJD_INPUT_KEYBOARD_COUNT,				// 0xD5 VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT
	RJD_INPUT_KEYBOARD_COUNT,				// 0xD6 VK_GAMEPAD_LEFT_THUMBSTICK_LEFT
	RJD_INPUT_KEYBOARD_COUNT,				// 0xD7 VK_GAMEPAD_RIGHT_THUMBSTICK_UP
	RJD_INPUT_KEYBOARD_COUNT,				// 0xD8 VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN
	RJD_INPUT_KEYBOARD_COUNT,				// 0xD9 VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT
	RJD_INPUT_KEYBOARD_COUNT,				// 0xDA VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '[',	// 0xDB VK_OEM_4  '[{' for US
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '\\',	// 0xDC VK_OEM_5  '\|' for US
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + ']',	// 0xDD VK_OEM_6  ']}' for US
	RJD_INPUT_KEYBOARD_ASCII_BEGIN + '"',	// 0xDE VK_OEM_7  ''"' for US
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xDF VK_OEM_8
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xE0 reserved
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xE1 VK_OEM_AX
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xE2 VK_OEM_102
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xE3 VK_ICO_HELP
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xE4 VK_ICO_00
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xE5 VK_PROCESSKEY
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xE6 VK_ICO_CLEAR
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xE7 VK_PACKET
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xE9 VK_OEM_RESET
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xEA VK_OEM_JUMP
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xEB VK_OEM_PA1
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xEC VK_OEM_PA2
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xED VK_OEM_PA3
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xEE VK_OEM_WSCTRL
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xEF VK_OEM_CUSEL
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xF0 VK_OEM_ATTN
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xF1 VK_OEM_FINISH
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xF2 VK_OEM_COPY
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xF3 VK_OEM_AUTO
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xF4 VK_OEM_ENLW
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xF5 VK_OEM_BACKTAB
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xF6 VK_ATTN
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xF7 VK_CRSEL
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xF8 VK_EXSEL
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xF9 VK_EREOF
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xFA VK_PLAY
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xFB VK_ZOOM
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xFC VK_NONAME
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xFD VK_PA1
	RJD_INPUT_KEYBOARD_COUNT, 				// 0xFE VK_OEM_CLEAR
};

////////////////////////////////////////////////////////////////////////////////
// public implementation

void rjd_input_create(struct rjd_input* out, struct rjd_mem_allocator* allocator)
{
	RJD_UNUSED_PARAM(allocator);
	memset(out, 0, sizeof(*out));
}

void rjd_input_destroy(struct rjd_input* input)
{
	rjd_input_unhook(input);
	memset(input, 0, sizeof(*input));
}

struct rjd_result rjd_input_hook(struct rjd_input* input, const struct rjd_window* window, const struct rjd_window_environment* env)
{
	RJD_ASSERT(input);
	RJD_ASSERT(window);
	RJD_ASSERT(env);

	void* hinstance = env->win32.hinstance;
	void* hwnd = rjd_window_win32_get_hwnd(window);

	if (GetWindowLongPtrW(hwnd, RJD_INPUT_WIN32_WINDOW_PTR_INDEX) != 0) {
		return RJD_RESULT("Another system aleady has the slot for RJD_INPUT_WIN32_WINDOW_PTR_INDEX.");
	}
	
	SetWindowLongPtrW(hwnd, RJD_INPUT_WIN32_WINDOW_PTR_INDEX, (LONG_PTR)input);

	struct rjd_input_win32* input_win32 = (struct rjd_input_win32*)input;

	input_win32->window = window;
	input_win32->hook_handle_wndproc = SetWindowsHookExW(WH_CALLWNDPROC, rjd_input_wndproc_hook, hinstance, GetCurrentThreadId());
	if (input_win32->hook_handle_wndproc == NULL) {
		return RJD_RESULT("Failed to hook window proc. Check GetLastError() for more info.");
	}

	input_win32->hook_handle_getmessage = SetWindowsHookExW(WH_GETMESSAGE, rjd_input_getmessage_hook, hinstance, GetCurrentThreadId());
	if (input_win32->hook_handle_getmessage == NULL) {
		return RJD_RESULT("Failed to hook getmessage. Check GetLastError() for more info.");
	}

	return RJD_RESULT_OK();
}

void rjd_input_unhook(struct rjd_input* input)
{
	struct rjd_input_win32* input_win32 = (struct rjd_input_win32*)input;
	if (input_win32->hook_handle_wndproc || input_win32->hook_handle_getmessage) {
		void* hwnd = rjd_window_win32_get_hwnd(input_win32->window);
		SetWindowLongPtrW(hwnd, RJD_INPUT_WIN32_WINDOW_PTR_INDEX, 0);
		UnhookWindowsHookEx(input_win32->hook_handle_wndproc);
		UnhookWindowsHookEx(input_win32->hook_handle_getmessage);

		input_win32->hook_handle_wndproc = NULL;
		input_win32->hook_handle_getmessage = NULL;
	}
}

void rjd_input_markframe(struct rjd_input* input)
{
	struct rjd_input_win32* input_win32 = (struct rjd_input_win32*)input;
	rjd_input_markframe_common(&input_win32->common);
}

bool rjd_input_keyboard_now(const struct rjd_input* input, enum rjd_input_keyboard code)
{
	struct rjd_input_win32* input_win32 = (struct rjd_input_win32*)input;
	return rjd_input_keyboard_now_common(&input_win32->common, code);
}

bool rjd_input_keyboard_prev(const struct rjd_input* input, enum rjd_input_keyboard code)
{
	struct rjd_input_win32* input_win32 = (struct rjd_input_win32*)input;
	return rjd_input_keyboard_prev_common(&input_win32->common, code);
}

float rjd_input_mouse_now(const struct rjd_input* input, enum rjd_input_mouse code)
{
	struct rjd_input_win32* input_win32 = (struct rjd_input_win32*)input;
	return rjd_input_mouse_now_common(&input_win32->common, code);
}

float rjd_input_mouse_prev(const struct rjd_input* input, enum rjd_input_mouse code)
{
	struct rjd_input_win32* input_win32 = (struct rjd_input_win32*)input;
	return rjd_input_mouse_prev_common(&input_win32->common, code);
}

void rjd_input_simulate(struct rjd_input* input, struct rjd_input_sim_event event)
{
	struct rjd_input_win32* input_win32 = (struct rjd_input_win32*)input;
	void* hwnd = rjd_window_win32_get_hwnd(input_win32->window);

	UINT msg = 0;
	WPARAM wparam = {0};
	LPARAM lparam = {0};

	switch (event.type)
	{
		case RJD_INPUT_SIM_TYPE_KEYBOARD:
		{
			msg = event.keyboard.is_down ? WM_KEYDOWN : WM_KEYUP;

			bool found = false;
			for (uint32_t i = 0; i < rjd_countof(RJD_INPUT_WIN32_KEYCODE_TO_ENUM); ++i) {
				if (RJD_INPUT_WIN32_KEYCODE_TO_ENUM[i] == event.keyboard.key) {
					wparam = i;
					found = true;
					break;
				}
			}
			RJD_ASSERTMSG(found, "No virtual key lookup found for enum rjd_input_keyboard value %d", event.keyboard.key);
			break;
		}

		case RJD_INPUT_SIM_TYPE_MOUSE:
		{
			switch (event.mouse.button)
			{
				case RJD_INPUT_MOUSE_X:
				{
					msg = WM_MOUSEMOVE;
					int16_t x = (int16_t)event.mouse.value;
					int16_t y = (int16_t)rjd_input_mouse_now(input, RJD_INPUT_MOUSE_Y);
					lparam = (y << 16) | (x);
					break;
				}
				case RJD_INPUT_MOUSE_Y:
				{
					msg = WM_MOUSEMOVE;
					int16_t x = (int16_t)rjd_input_mouse_now(input, RJD_INPUT_MOUSE_X);
					int16_t y = (int16_t)event.mouse.value;
					lparam = (y << 16) | (x);
					break;
				}
				case RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_X:
				case RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_Y:
				{
					msg = WM_MOUSEWHEEL;
					wparam = (uint16_t)(event.mouse.value * WHEEL_DELTA) << 16;
					break;
				}
				case RJD_INPUT_MOUSE_BUTTON_LEFT:
				{
					msg = (event.mouse.value == 0.0f) ? WM_LBUTTONUP : WM_LBUTTONDOWN;
					break;
				}
				case RJD_INPUT_MOUSE_BUTTON_RIGHT:
				{
					msg = (event.mouse.value == 0.0f) ? WM_LBUTTONUP : WM_LBUTTONDOWN;
					break;
				}
				default:
				{
					if (event.mouse.button == RJD_INPUT_MOUSE_BUTTON_BEGIN + 2) {
						msg = (event.mouse.value == 0.0f) ? WM_MBUTTONUP : WM_MBUTTONDOWN;
					} else if (event.mouse.button == RJD_INPUT_MOUSE_BUTTON_BEGIN + 3) {
						msg = (event.mouse.value == 0.0f) ? WM_XBUTTONUP : WM_XBUTTONDOWN;
						wparam = XBUTTON1 << 16;
					} else if (event.mouse.button == RJD_INPUT_MOUSE_BUTTON_BEGIN + 4) {
						msg = (event.mouse.value == 0.0f) ? WM_XBUTTONUP : WM_XBUTTONDOWN;
						wparam = XBUTTON2 << 16;
					}
				}
			}
		}
	}

	SendMessageW(hwnd, msg, wparam, lparam);
}

////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK rjd_input_wndproc_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CWPSTRUCT* msg = (const CWPSTRUCT*)lParam;

	struct rjd_input_win32* input_win32 = (struct rjd_input_win32*)GetWindowLongPtrW(msg->hwnd, RJD_INPUT_WIN32_WINDOW_PTR_INDEX);
	if (nCode < 0 || input_win32 == NULL) {
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	};

	return rjd_input_hook_common(input_win32, msg->message, msg->wParam, msg->lParam, nCode, wParam, lParam);
}

LRESULT CALLBACK rjd_input_getmessage_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
	const MSG* msg = (const MSG*)lParam;

	struct rjd_input_win32* input_win32 = (struct rjd_input_win32*)GetWindowLongPtrW(msg->hwnd, RJD_INPUT_WIN32_WINDOW_PTR_INDEX);
	if (nCode < 0 || input_win32 == NULL) {
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	};

	return rjd_input_hook_common(input_win32, msg->message, msg->wParam, msg->lParam, nCode, wParam, lParam);
}

LRESULT rjd_input_hook_common(struct rjd_input_win32* input_win32, UINT message, WPARAM wParam, LPARAM lParam, int nCode, WPARAM hook_wparam, LPARAM hook_lparam)
{
	const uint32_t now_index = input_win32->common.now_index;

	switch (message)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			// const uint32_t repeat_count = (0xFFFF & lParam);
			const uint32_t scan_code = (0xFF0000 & lParam) >> 16;
			const bool is_extended = (0x01000000 & lParam) != 0;
			const bool is_down = (message == WM_KEYDOWN);

			uint32_t virtual_key = (uint32_t)wParam;
			if (wParam == VK_SHIFT) {
				virtual_key = MapVirtualKey(scan_code, MAPVK_VSC_TO_VK_EX);
			} else if (wParam == VK_CONTROL) {
				virtual_key = is_extended ? VK_RCONTROL : VK_LCONTROL;
			} else if (wParam == VK_MENU) {
				virtual_key = is_extended ? VK_RMENU : VK_LMENU;
			}

			if (virtual_key < rjd_countof(RJD_INPUT_WIN32_KEYCODE_TO_ENUM)) {
				enum rjd_input_keyboard key = RJD_INPUT_WIN32_KEYCODE_TO_ENUM[virtual_key];

				input_win32->common.keyboard[now_index].values[key] = is_down;
			}
			break;
		}
		case WM_LBUTTONDOWN:
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_BUTTON_LEFT] = 1.0f;
			break;
		case WM_LBUTTONUP:
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_BUTTON_LEFT] = 0.0f;
			break;
		case WM_RBUTTONDOWN:
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_BUTTON_RIGHT] = 1.0f;
			break;
		case WM_RBUTTONUP:
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_BUTTON_RIGHT] = 0.0f;
			break;
		case WM_MBUTTONDOWN:
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_BUTTON_BEGIN + 2] = 1.0f;
			break;
		case WM_MBUTTONUP:
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_BUTTON_BEGIN + 2] = 0.0f;
			break;
		case WM_XBUTTONDOWN:
		{
			uint32_t button_index = (((wParam & 0xFFFF0000) >> 16) == XBUTTON1) ? 3 : 4;
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_BUTTON_BEGIN + button_index] = 1.0f;
			break;
		}
		case WM_XBUTTONUP:
		{
			uint32_t button_index = (((wParam & 0xFFFF0000) >> 16) == XBUTTON1) ? 3 : 4;
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_BUTTON_BEGIN + button_index] = 0.0f;
			break;
		}
		case WM_MOUSEWHEEL:
		{
			float delta = (float)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_Y] = delta;
			break;
		}
		case WM_MOUSEHWHEEL:
		{
			float delta = (float)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_X] = delta;
			break;
		}
		case WM_MOUSEMOVE:
		{
			float x = (float)GET_X_LPARAM(lParam);
			float y = (float)GET_Y_LPARAM(lParam);
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_X] = x;
			input_win32->common.mouse[now_index].values[RJD_INPUT_MOUSE_Y] = y;
			break;
		}
	}

	return CallNextHookEx(NULL, nCode, hook_wparam, hook_lparam);
}

#elif RJD_PLATFORM_OSX

#include <Carbon/Carbon.h>

////////////////////////////////////////////////////////////////////////////////
// Local helpers

struct rjd_input_osx;

@interface InputResponder : NSResponder
	@property(readonly) BOOL acceptsFirstResponder;
	-(instancetype)initWithInput:(struct rjd_input_osx*)_input;
@end

struct rjd_input_osx
{
	InputResponder* responder;
	const struct rjd_window* window;

	struct rjd_input_common common;
};

RJD_STATIC_ASSERT(sizeof(struct rjd_input_osx) <= sizeof(struct rjd_input));

////////////////////////////////////////////////////////////////////////////////
// Local data

const uint8_t RJD_INPUT_OSX_KEYCODE_TO_ENUM[] =
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

	NSView* view = rjd_window_osx_get_mtkview(window);
	if (view == nil) {
		view = rjd_window_osx_get_basicview(window);
		if (view == nil) {
			return RJD_RESULT("No view available in the window to hook. Did the window initialize correctly?");
		}
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
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;
	rjd_input_markframe_common(&input_osx->common);
}

bool rjd_input_keyboard_now(const struct rjd_input* input, enum rjd_input_keyboard code)
{
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;
	return rjd_input_keyboard_now_common(&input_osx->common, code);
}

bool rjd_input_keyboard_prev(const struct rjd_input* input, enum rjd_input_keyboard code)
{
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;
	return rjd_input_keyboard_prev_common(&input_osx->common, code);
}

float rjd_input_mouse_now(const struct rjd_input* input, enum rjd_input_mouse code)
{
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;
	return rjd_input_mouse_now_common(&input_osx->common, code);
}

float rjd_input_mouse_prev(const struct rjd_input* input, enum rjd_input_mouse code)
{
	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;
	return rjd_input_mouse_prev_common(&input_osx->common, code);
}

void rjd_input_simulate(struct rjd_input* input, struct rjd_input_sim_event event)
{
	RJD_ASSERT(input);

	struct rjd_input_osx* input_osx = (struct rjd_input_osx*)input;

	switch (event.type)
	{
		case RJD_INPUT_SIM_TYPE_KEYBOARD:
		{
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
                for (int32_t i = 0; i < (int32_t)rjd_countof(RJD_INPUT_OSX_KEYCODE_TO_ENUM); ++i) {
                    if (RJD_INPUT_OSX_KEYCODE_TO_ENUM[i] == event.keyboard.key) {
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
	if (event.keyCode < rjd_countof(RJD_INPUT_OSX_KEYCODE_TO_ENUM)) {
		enum rjd_input_keyboard code = RJD_INPUT_OSX_KEYCODE_TO_ENUM[event.keyCode];
		input->common.keyboard[input->common.now_index].values[code] = 1;
	}
}

-(void)keyUp:(NSEvent*)event
{
	if (event.keyCode < rjd_countof(RJD_INPUT_OSX_KEYCODE_TO_ENUM)) {
		enum rjd_input_keyboard code = RJD_INPUT_OSX_KEYCODE_TO_ENUM[event.keyCode];
		input->common.keyboard[input->common.now_index].values[code] = 0;
	}
}

-(void)flagsChanged:(NSEvent*)event
{
	struct rjd_input_keyboard_state* state = input->common.keyboard + input->common.now_index;

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
    input->common.mouse[input->common.now_index].values[RJD_INPUT_MOUSE_BUTTON_LEFT] = 1;
}

-(void)mouseUp:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
    input->common.mouse[input->common.now_index].values[RJD_INPUT_MOUSE_BUTTON_LEFT] = 0;
}
                  
-(void)rightMouseDown:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
	input->common.mouse[input->common.now_index].values[RJD_INPUT_MOUSE_BUTTON_RIGHT] = 1;
}

-(void)rightMouseUp:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
	input->common.mouse[input->common.now_index].values[RJD_INPUT_MOUSE_BUTTON_RIGHT] = 0;
}
                  
-(void)otherMouseDown:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
	for (uint32_t button = RJD_INPUT_MOUSE_BUTTON_BEGIN; button < RJD_INPUT_MOUSE_BUTTON_END; ++button) {
		uint32_t bit = 1 << (button - RJD_INPUT_MOUSE_BUTTON_BEGIN);
		bool pressed = (NSEvent.pressedMouseButtons & bit) != 0;
		input->common.mouse[input->common.now_index].values[button] = pressed;
	}
}

-(void)otherMouseUp:(NSEvent*)event
{
	RJD_UNUSED_PARAM(event);
	for (uint32_t button = RJD_INPUT_MOUSE_BUTTON_BEGIN; button < RJD_INPUT_MOUSE_BUTTON_END; ++button) {
		uint32_t bit = 1 << (button - RJD_INPUT_MOUSE_BUTTON_BEGIN);
		bool pressed = (NSEvent.pressedMouseButtons & bit) != 0;
		input->common.mouse[input->common.now_index].values[button] = pressed;
	}
}

-(void)mouseMoved:(NSEvent*)event
{
	NSView* view = rjd_window_osx_get_mtkview(input->window);
    if (view == nil) {
        view = rjd_window_osx_get_basicview(input->window);
    }
	NSPoint locationInView = [view convertPoint:event.locationInWindow fromView:nil];

	int x = locationInView.x;
	int y = locationInView.y;

	struct rjd_window_size size = rjd_window_size_get(input->window);
	if (x >= 0 && x <= size.width && y >= 0 && y <= size.height) {
		input->common.mouse[input->common.now_index].values[RJD_INPUT_MOUSE_X] = (float)x;
		input->common.mouse[input->common.now_index].values[RJD_INPUT_MOUSE_Y] = (float)y;
	}
}

-(void)scrollWheel:(NSEvent*)event
{
	input->common.mouse[input->common.now_index].values[RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_X] = (float)event.scrollingDeltaX;
	input->common.mouse[input->common.now_index].values[RJD_INPUT_MOUSE_SCROLLWHEEL_DELTA_Y] = (float)event.scrollingDeltaY;
}

@end

#endif // RJD_PLATFORM_*
#endif // RJD_IMPL
