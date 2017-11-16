#pragma once

#define RJD_RNG

struct rjd_rng
{
	uint64_t seed;
	uint64_t state;
};

struct rjd_rng rjd_rng_init(uint64_t seed);
uint64_t rjd_rng_next(struct rjd_rng* rng);
double rjd_rng_float(struct rjd_rng* rng);
int32_t rjd_rng_range32(struct rjd_rng* rng, int32_t min_inclusive, int32_t max_exclusive);

#if RJD_ENABLE_SHORTNAMES
	#define rng_init	rjd_rng_init
	#define rng_next	rjd_rng_next
	#define rng_float	rjd_rng_float
	#define rng_range32 rjd_rng_range32
#endif

#if RJD_IMPL

struct rjd_rng rjd_rng_init(uint64_t seed)
{
	struct rjd_rng rng = { seed, seed };
	return rng;
}

uint64_t rjd_rng_next(struct rjd_rng* rng)
{
	rng->state ^= rng->state << 15;
	rng->state ^= rng->state >> 3;
	rng->state ^= rng->state << 52;
	return rng->state;
}

double rjd_rng_float(struct rjd_rng* rng)
{
	uint64_t next = rjd_rng_next(rng) % 1000001ull;
	return (double)next / 1000000.0;
}

int32_t rjd_rng_range32(struct rjd_rng* rng, int32_t min_inclusive, int32_t max_exclusive)
{
	if (max_exclusive < min_inclusive) {
		return max_exclusive;
	}

	int32_t range = max_exclusive - min_inclusive;
	if (range == 0) {
		return min_inclusive;
	}

	uint64_t next = rjd_rng_next(rng);
	return min_inclusive + (next % range);
}

#endif // RJD_IMPL

