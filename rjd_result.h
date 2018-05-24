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

