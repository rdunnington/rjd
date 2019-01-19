#pragma once

#define RJD_RESULT_H 1

struct rjd_result
{
	const char* error;
};

static inline struct rjd_result rjd_result_init(const char* message) {
	return (struct rjd_result){message};
}

static inline bool rjd_result_isok(struct rjd_result result) {
	return result.error == NULL;
}

// TODO static assert that message is a compile-time string
#define RJD_RESULT(message) rjd_result_init(message)
#define RJD_RESULT_OK() rjd_result_init(NULL)
#define RJD_RESULT_CHECK(validation_condition, message) if (!(validation_condition)) { return RJD_RESULT(message); }
#define RJD_RESULT_PROMOTE(result) if (!rjd_result_isok(result)) { return result; }
