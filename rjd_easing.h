#pragma once

#define RJD_EASING

enum rjd_ease_type
{
	RJD_EASE_TYPE_LINE, // linear
	RJD_EASE_TYPE_SINE, // sine
	RJD_EASE_TYPE_CUBE, // cubic
	RJD_EASE_TYPE_QUAD, // quadratic
	RJD_EASE_TYPE_QUAR, // quartic
	RJD_EASE_TYPE_QUIN, // quintic
	RJD_EASE_TYPE_EXPO, // exponential
	RJD_EASE_TYPE_CIRC, // circular
	RJD_EASE_TYPE_BACK, // back
	RJD_EASE_TYPE_ELAS, // elastic
	RJD_EASE_TYPE_BOUN, // bounce
	RJD_EASE_TYPE_MAX,
};

enum rjd_ease_dir
{
	RJD_EASE_DIR_INOUT,
	RJD_EASE_DIR_IN,
	RJD_EASE_DIR_OUT,
	RJD_EASE_DIR_MAX,
};

static inline float rjd_ease(float t, enum rjd_ease_type type, enum rjd_ease_dir dir);
static inline float rjd_ease_between(float t, float min, float max);
static inline float rjd_ease_line(float t);
static inline float rjd_ease_in_sine(float t);
static inline float rjd_ease_in_quad(float t);
static inline float rjd_ease_in_cube(float t);
static inline float rjd_ease_in_quar(float t);
static inline float rjd_ease_in_quin(float t);
static inline float rjd_ease_in_expo(float t);
static inline float rjd_ease_in_circ(float t);
static inline float rjd_ease_in_back(float t);
static inline float rjd_ease_in_elas(float t);
static inline float rjd_ease_in_boun(float t);
static inline float rjd_ease_out_sine(float t);
static inline float rjd_ease_out_quad(float t);
static inline float rjd_ease_out_cube(float t);
static inline float rjd_ease_out_quar(float t);
static inline float rjd_ease_out_quin(float t);
static inline float rjd_ease_out_expo(float t);
static inline float rjd_ease_out_circ(float t);
static inline float rjd_ease_out_back(float t);
static inline float rjd_ease_out_elas(float t);
static inline float rjd_ease_out_boun(float t);
static inline float rjd_ease_inout_sine(float t);
static inline float rjd_ease_inout_quad(float t);
static inline float rjd_ease_inout_cube(float t);
static inline float rjd_ease_inout_quar(float t);
static inline float rjd_ease_inout_quin(float t);
static inline float rjd_ease_inout_expo(float t);
static inline float rjd_ease_inout_circ(float t);
static inline float rjd_ease_inout_back(float t);
static inline float rjd_ease_inout_elas(float t);
static inline float rjd_ease_inout_boun(float t);

#ifdef RJD_ENABLE_SHORTNAMES
	#define ease            rjd_ease
	#define ease_between	rjd_ease_between
	#define ease_line       rjd_ease_line
	#define ease_in_sine    rjd_ease_in_sine
	#define ease_in_quad    rjd_ease_in_quad
	#define ease_in_cube    rjd_ease_in_cube
	#define ease_in_quar    rjd_ease_in_quar
	#define ease_in_quin    rjd_ease_in_quin
	#define ease_in_expo    rjd_ease_in_expo
	#define ease_in_circ    rjd_ease_in_circ
	#define ease_in_back    rjd_ease_in_back
	#define ease_in_elas    rjd_ease_in_elas
	#define ease_in_boun    rjd_ease_in_boun
	#define ease_out_sine   rjd_ease_out_sine
	#define ease_out_quad   rjd_ease_out_quad
	#define ease_out_cube   rjd_ease_out_cube
	#define ease_out_quar   rjd_ease_out_quar
	#define ease_out_quin   rjd_ease_out_quin
	#define ease_out_expo   rjd_ease_out_expo
	#define ease_out_circ   rjd_ease_out_circ
	#define ease_out_back   rjd_ease_out_back
	#define ease_out_elas   rjd_ease_out_elas
	#define ease_out_boun   rjd_ease_out_boun
	#define ease_inout_sine rjd_ease_inout_sine
	#define ease_inout_quad rjd_ease_inout_quad
	#define ease_inout_cube rjd_ease_inout_cube
	#define ease_inout_quar rjd_ease_inout_quar
	#define ease_inout_quin rjd_ease_inout_quin
	#define ease_inout_expo rjd_ease_inout_expo
	#define ease_inout_circ rjd_ease_inout_circ
	#define ease_inout_back rjd_ease_inout_back
	#define ease_inout_elas rjd_ease_inout_elas
	#define ease_inout_boun rjd_ease_inout_boun#endif

	#define EASE_TYPE_LINE RJD_EASE_TYPE_LINE
	#define EASE_TYPE_SINE RJD_EASE_TYPE_SINE
	#define EASE_TYPE_CUBE RJD_EASE_TYPE_CUBE
	#define EASE_TYPE_QUAD RJD_EASE_TYPE_QUAD
	#define EASE_TYPE_QUAR RJD_EASE_TYPE_QUAR
	#define EASE_TYPE_QUIN RJD_EASE_TYPE_QUIN
	#define EASE_TYPE_EXPO RJD_EASE_TYPE_EXPO
	#define EASE_TYPE_CIRC RJD_EASE_TYPE_CIRC
	#define EASE_TYPE_BACK RJD_EASE_TYPE_BACK
	#define EASE_TYPE_ELAS RJD_EASE_TYPE_ELAS
	#define EASE_TYPE_BOUN RJD_EASE_TYPE_BOUN
	#define EASE_TYPE_MAX  RJD_EASE_TYPE_MAX

	#define EASE_DIR_INOUT RJD_EASE_DIR_INOUT
	#define EASE_DIR_IN    RJD_EASE_DIR_IN
	#define EASE_DIR_OUT   RJD_EASE_DIR_OUT
	#define EASE_DIR_MAX   RJD_EASE_DIR_MAX
#endif

// impl

static inline float rjd_ease(float t, enum rjd_ease_type type, enum rjd_ease_dir dir)
{
	switch (dir) {
		case RJD_EASE_DIR_IN: {
			switch(type) {
				case RJD_EASE_TYPE_LINE: return rjd_ease_line(t);
				case RJD_EASE_TYPE_SINE: return rjd_ease_in_sine(t);
				case RJD_EASE_TYPE_CUBI: return rjd_ease_in_cubi(t);
				case RJD_EASE_TYPE_QUAD: return rjd_ease_in_quad(t);
				case RJD_EASE_TYPE_QUAR: return rjd_ease_in_quar(t);
				case RJD_EASE_TYPE_QUIN: return rjd_ease_in_quin(t);
				case RJD_EASE_TYPE_EXPO: return rjd_ease_in_expo(t);
				case RJD_EASE_TYPE_CIRC: return rjd_ease_in_circ(t);
				case RJD_EASE_TYPE_BACK: return rjd_ease_in_back(t);
				case RJD_EASE_TYPE_ELAS: return rjd_ease_in_elas(t);
				case RJD_EASE_TYPE_BOUN: return rjd_ease_in_boun(t);
				case RJD_EASE_TYPE_MAX:  break;
			}
		}

		case RJD_EASE_DIR_OUT: {
			switch(type) {
				case RJD_EASE_TYPE_LINE: return rjd_ease_line(t);
				case RJD_EASE_TYPE_SINE: return rjd_ease_inout_sine(t);
				case RJD_EASE_TYPE_CUBI: return rjd_ease_inout_cubi(t);
				case RJD_EASE_TYPE_QUAD: return rjd_ease_inout_quad(t);
				case RJD_EASE_TYPE_QUAR: return rjd_ease_inout_quar(t);
				case RJD_EASE_TYPE_QUIN: return rjd_ease_inout_quin(t);
				case RJD_EASE_TYPE_EXPO: return rjd_ease_inout_expo(t);
				case RJD_EASE_TYPE_CIRC: return rjd_ease_inout_circ(t);
				case RJD_EASE_TYPE_BACK: return rjd_ease_inout_back(t);
				case RJD_EASE_TYPE_ELAS: return rjd_ease_inout_elas(t);
				case RJD_EASE_TYPE_BOUN: return rjd_ease_inout_boun(t);
				case RJD_EASE_TYPE_MAX:  break;
			}
		}

		case RJD_EASE_DIR_INOUT: {
			switch(type) {
				case RJD_EASE_TYPE_LINE: return rjd_ease_line(t);
				case RJD_EASE_TYPE_SINE: return rjd_ease_inout_sine(t);
				case RJD_EASE_TYPE_CUBI: return rjd_ease_inout_cubi(t);
				case RJD_EASE_TYPE_QUAD: return rjd_ease_inout_quad(t);
				case RJD_EASE_TYPE_QUAR: return rjd_ease_inout_quar(t);
				case RJD_EASE_TYPE_QUIN: return rjd_ease_inout_quin(t);
				case RJD_EASE_TYPE_EXPO: return rjd_ease_inout_expo(t);
				case RJD_EASE_TYPE_CIRC: return rjd_ease_inout_circ(t);
				case RJD_EASE_TYPE_BACK: return rjd_ease_inout_back(t);
				case RJD_EASE_TYPE_ELAS: return rjd_ease_inout_elas(t);
				case RJD_EASE_TYPE_BOUN: return rjd_ease_inout_boun(t);
				case RJD_EASE_TYPE_MAX:  break;
			}
		}

		default: break;
	}

	RJD_ASSERTFAIL("type (%d) or dir (%d) was invalid.\n", type, dir);
	return 0;
}

static inline float rjd_ease_between(float t, float min, float max, rjd_ease_func f) {
	return f(t) * (max - min) + min;
}

static inline float rjd_ease_line(float t) {
	return t;
}

static inline float rjd_ease_in_sine(float t) {
	return cosf(t*RJD_MATH_PI/2.0f);
}

static inline float rjd_ease_in_quad(float t) {
	return t*t;
}

static inline float rjd_ease_in_cube(float t) {
	return t*t*t;
}

static inline float rjd_ease_in_quar(float t) {
	return t*t*t*t;
}

static inline float rjd_ease_in_quin(float t) {
	return t*t*t*t*t;
}

static inline float rjd_ease_in_expo(float t) {
	return t == 0 ? t : powf(2, -10*(t - 1));
}

static inline float rjd_ease_in_circ(float t) {
	return -sqrtf(1-t*t) + 1;
}

static inline float rjd_ease_in_back(float t) {
	return t * t * t - t * sinf(t * RJD_MATH_PI);
}

static inline float rjd_ease_in_elas(float t) {
	return sinf(13.0f * RJD_MATH_PI / 2.0f * t) * powf(2, 10 * (t - 1));
}

static inline float rjd_ease_in_boun(float t) {
	return 1 - rjd_ease_out_boun(1 - t);
}

static inline float rjd_ease_out_sine(float t) {
	return sinf(t*RJD_MATH_PI/2.0f);
}

static inline float rjd_ease_out_quad(float t) {
	float tt = t - 1;
	return 1 - (tt * tt);
}

static inline float rjd_ease_out_cube(float t) {
	float tt = t - 1;
	return tt * tt * tt + 1;
}

static inline float rjd_ease_out_quar(float t) {
	float tt = t - 1;
	return 1 - (tt * tt * tt * tt);
}

static inline float rjd_ease_out_quin(float t) {
	float tt = t - 1;
	return tt * tt * tt * tt * tt + 1;
}

static inline float rjd_ease_out_expo(float t) {
	return t == 1 ? t : -powf(2, -10*t) + 1;
}

static inline float rjd_ease_out_circ(float t) {
	float tt = t - 1;
	return sqrtf(1 - tt * tt)
}

static inline float rjd_ease_out_back(float t) {
	float tt = 1 - t;
	return tt*tt*tt - tt*sinf(tt*RJD_MATH_PI);
}

static inline float rjd_ease_out_elas(float t) {
	return sin(-13.0f * RJD_MATH_PI / 2.0f * (t + 1)) * pow(2, -10 * t + 1);
}

static inline float rjd_ease_out_boun(float t) {
	if (t < 4.0f/11.0f) {
		return 121 * t * t / 16.0f;
	} else if (t < 8.0f/11.0f) {
		return 363.0f/40.0f*t*t - 99.0f/10.0f*t + 17.0f/5.0f;
	} else if (t < 9.0f/10.0f) {
		return 4356.0f/361.0f*t*t - 3544230f/1805.0f*t + 16061.0f/1805.0f;
	} else {
		return 54.0f/5.0f*t*t - 513.0f/25.0f*t + 268.0/25.0f;
	}
}

static inline float rjd_ease_inout_sine(float t) {
	return sinf(t*RJD_MATH_PI - RJD_MATH_PI/2.0f) / 2.0f + 0.5f;
}

static inline float rjd_ease_inout_quad(float t) {
	float tt = rjd_math_remapf(t,0,1,0,2);
	return (tt < 1) ? rjd_ease_in_quad(t) : rjd_ease_out_quad(t - 1);
}

static inline float rjd_ease_inout_cube(float t) {
	float tt = rjd_math_remapf(t,0,1,0,2);
	return (tt < 1) ? rjd_ease_in_cube(t) : rjd_ease_out_cube(t - 1);
}

static inline float rjd_ease_inout_quar(float t) {
	float tt = rjd_math_remapf(t,0,1,0,2);
	return (tt < 1) ? rjd_ease_in_quar(t) : rjd_ease_out_quar(t - 1);
}

static inline float rjd_ease_inout_quin(float t) {
	float tt = rjd_math_remapf(t,0,1,0,2);
	return (tt < 1) ? rjd_ease_in_quin(t) : rjd_ease_out_quin(t - 1);
}

static inline float rjd_ease_inout_expo(float t) {
	float tt = rjd_math_remapf(t,0,1,0,2);
	return (tt < 1) ? rjd_ease_in_expo(t) : rjd_ease_out_expo(t - 1);
}

static inline float rjd_ease_inout_circ(float t) {
	float tt = rjd_math_remapf(t,0,1,0,2);
	return (tt < 1) ? rjd_ease_in_circ(t) : rjd_ease_out_circ(t - 1);
}

static inline float rjd_ease_inout_back(float t) {
	float tt = rjd_math_remapf(t,0,1,0,2);
	return (tt < 1) ? rjd_ease_in_back(t) : rjd_ease_out_back(t - 1);
}

static inline float rjd_ease_inout_elas(float t) {
	float tt = rjd_math_remapf(t,0,1,0,2);
	return (tt < 1) ? rjd_ease_in_elas(t) : rjd_ease_out_elas(t - 1);
}

static inline float rjd_ease_inout_boun(float t) {
	float tt = rjd_math_remapf(t,0,1,0,2);
	return (tt < 1) ? rjd_ease_in_boun(t) : rjd_ease_out_boun(t - 1);
}

