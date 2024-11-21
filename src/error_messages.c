#include <errno.h>
#include <string.h>

#include "error_codes.h"
#include "error_messages.h"

// --------

const char* get_error_message(int error_code) {

	switch(error_code) {

		case ERROR_CODE_UNKNOWN_SYSTEM_ERROR:
			return "Unknown system error";

		case ERROR_CODE_INTEGER_OVERFLOW:
			return strerror(ERANGE);

		case ERROR_CODE_MALLOC_FAILURE:
			return strerror(ENOMEM);

		case ERROR_CODE_OPEN_FAILURE:
			return "Cannot open";

		case ERROR_CODE_SDL_ERROR:
			return "SDL error";

		case ERROR_CODE_SUCCESS:
			return "Success";

		case ERROR_CODE_INVALID_STATE:
			return "Encountered invalid state";

		case ERROR_CODE_INVALID_ARGUMENT:
			return strerror(EINVAL);

		case ERROR_CODE_UNEXPECTED_EOF:
			return "Unexpected end of file";

		case ERROR_CODE_UNEXPECTED_CHARACTER:
			return "Unexpected character";

		case ERROR_CODE_UNEXPECTED_FOLLOW_UP_CHARACTER:
			return "Unexpected follow-up character";

		case ERROR_CODE_BAR_TOO_SHORT:
			return "Bar too short";

		case ERROR_CODE_BAR_TOO_LONG:
			return "Bar too long";

		default:
			return "Unknown error";
	}
}

