#pragma once

#define RJD_RESULT_H 1

struct rjd_result
{
	const char* error;
};

static inline bool rjd_result_isok(struct rjd_result result) {
	return result.error == NULL;
}

// TODO static assert that message is a compile-time string
#define RJD_RESULT(message) ((struct rjd_result){message})
#define RJD_RESULT_OK() RJD_RESULT(NULL)
#define RJD_RESULT_CHECK(validation_condition, message) if (!(validation_condition)) { return RJD_RESULT(message); }
#define RJD_RESULT_PROMOTE(result) if (!rjd_result_isok(result)) { return result; }
