#pragma once

#define RJD_UTF8_H 1

const char* rjd_utf8_bom_skip(const char* string); // expects a NULL-terminated string
struct rjd_result rjd_utf8_bom_write(char* buffer, size_t size);
const char* rjd_utf8_next(const char* string); // returns NULL if string is not UTF8-encoded

#if RJD_IMPL

const char* rjd_utf8_bom_skip(const char* string)
{
	const uint8_t* s = (const uint8_t*)string;
	if (s != NULL &&
		*(s + 0) == 0xEF &&
		*(s + 1) == 0xBB &&
		*(s + 2) == 0xBF)
	{
		return string + 3;
	}
	return string;
}

struct rjd_result rjd_utf8_bom_write(char* buffer, size_t size)
{
	if (size <= 3) {
		return RJD_RESULT("Buffer must be at least 3 characters long.");
	}

	*(buffer + 0) = 0xEF;
	*(buffer + 1) = 0xBB;
	*(buffer + 2) = 0xBF;

	return RJD_RESULT_OK();
}

const char* rjd_utf8_next(const char* string)
{
	if (string == NULL || *string == '\0') {
		return string;
	}

	uint8_t byte = (uint8_t)*string;

    if ((byte >> 7) == 0x00) {
        return string + 1;
    } else if ((byte >> 5) == 0x06) { // 0b110
        return string + 2;
    } else if ((byte >> 4) == 0x0E) { // 0b1110
        return string + 3;
    } else if ((byte >> 3) == 0x1E) { //0b11110
        return string + 4;
    }

	// if we're in the middle of a character, skip to the end of the current character
    bool is_mid_character = (byte >> 6) == 0x02; // 0b10
	if (is_mid_character) {
		while (is_mid_character) {
			++string;
			byte = *string;
			if (byte == '\0') {
				break;
			}
            is_mid_character = (byte >> 6) == 0x02; //0b10
		}
		return string;
	}

	return NULL; // invalid UTF8 string
}

#endif

