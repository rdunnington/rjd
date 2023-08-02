#pragma once

#define RJD_GEO_H 1

typedef struct {
	rjd_math_vec4 minmax; // xy is the min, zw is the max
} rjd_geo_rect;

typedef struct {
	rjd_math_vec3 xyr; // z is the radius
} rjd_geo_circle;

typedef struct {
	rjd_math_vec3 min;
	rjd_math_vec3 max;
} rjd_geo_box;

typedef struct {
	rjd_math_vec4 xyzr; // w is the radius
} rjd_geo_sphere;

typedef struct {
	rjd_math_vec3 p;
	rjd_math_vec3 d;
} rjd_geo_ray;

rjd_geo_rect rjd_geo_rect_minmax(float minx, float miny, float maxx, float maxy);
rjd_geo_circle rjd_geo_circle_xyr(float x, float y, float r);
rjd_geo_box rjd_geo_box_minmax(rjd_math_vec3 min,  rjd_math_vec3 max);
rjd_geo_sphere rjd_geo_sphere_xyzr(float x, float y, float z, float r);
rjd_geo_ray rjd_geo_ray_pd(rjd_math_vec3 p,  rjd_math_vec3 d);

bool rjd_geo_point_rect(rjd_math_vec3 p, rjd_geo_rect r);
bool rjd_geo_point_box(rjd_math_vec3 p, rjd_geo_box b);
bool rjd_geo_point_circle(rjd_math_vec3 p, rjd_geo_circle c);
bool rjd_geo_point_sphere(rjd_math_vec3 p, rjd_geo_sphere s);
bool rjd_geo_circle_circle(rjd_geo_circle c1, rjd_geo_circle c2);
bool rjd_geo_circle_rect(rjd_geo_circle c, rjd_geo_rect r);
bool rjd_geo_rect_rect(rjd_geo_rect a, rjd_geo_rect b);
bool rjd_geo_sphere_sphere(rjd_geo_sphere b, rjd_geo_sphere a);
bool rjd_geo_sphere_box(rjd_geo_sphere s, rjd_geo_box b);
bool rjd_geo_box_box(rjd_geo_box a, rjd_geo_box b);
bool rjd_geo_ray_point(rjd_geo_ray r, rjd_math_vec3 p, float* t_out);
bool rjd_geo_ray_sphere(rjd_geo_ray r, rjd_geo_sphere s, float* t_out);
bool rjd_geo_ray_box(rjd_geo_ray r, rjd_geo_box b, float* t_out);
bool rjd_geo_ray_boxfast(rjd_math_vec3 ray_pos, rjd_math_vec3 ray_inv_dir, rjd_geo_box b, float* t_out);

#if RJD_IMPL

rjd_geo_rect rjd_geo_rect_minmax(float minx, float miny, float maxx, float maxy) {
	RJD_ASSERT(minx <= maxx);
	RJD_ASSERT(miny <= maxy);
	rjd_geo_rect r = { rjd_math_vec4_xyzw(minx, miny, maxx, maxy) };
	return r;
}

rjd_geo_circle rjd_geo_circle_xyr(float x, float y, float r) {
	rjd_geo_circle c = { rjd_math_vec3_xyz(x,y,r) };
	return c;
}

rjd_geo_box rjd_geo_box_minmax(rjd_math_vec3 min, rjd_math_vec3 max) {
	rjd_geo_box b = { min, max };
	return b;
}

rjd_geo_sphere rjd_geo_sphere_xyzr(float x, float y, float z, float r) {
	rjd_geo_sphere s = { rjd_math_vec4_xyzw(x,y,z,r) };
	return s;
}

rjd_geo_ray rjd_geo_ray_pd(rjd_math_vec3 p, rjd_math_vec3 d) {
	rjd_geo_ray r = { p, d };
	return r;
}

bool rjd_geo_point_rect(rjd_math_vec3 p, rjd_geo_rect r) {
	rjd_math_vec4 pp = rjd_math_vec3to4(p);
	pp = rjd_math_vec4_shuffle(pp,0,1,0,1); // { x,y,x,y }

	rjd_math_vec4 a = { _mm_unpackhi_ps(pp.v, r.minmax.v) }; // { x maxx y maxy }
	rjd_math_vec4 b = { _mm_unpacklo_ps(pp.v, r.minmax.v) }; // { x minx y miny }
	b = rjd_math_vec4_shuffle(b,1,0,3,2);						 // { minx x miny y }
	
	return rjd_math_vec4_ge(a,b); // { x >= minx, maxx >= x, y >= miny, maxy >= y }
}

bool rjd_geo_point_box(rjd_math_vec3 p, rjd_geo_box b) {
	return rjd_math_vec3_ge(p, b.min) && rjd_math_vec3_ge(b.max, p);
}

bool rjd_geo_point_circle(rjd_math_vec3 p, rjd_geo_circle c) {
	rjd_math_vec3 v = rjd_math_vec3_sub(p, rjd_math_vec3_setz(c.xyr,0));
	return rjd_math_vec3_lengthsq(v) <= powf(rjd_math_vec3_z(c.xyr), 2);
}

bool rjd_geo_point_sphere(rjd_math_vec3 p, rjd_geo_sphere s) {
	rjd_math_vec4 pp = rjd_math_vec3to4(p);
	rjd_math_vec4 v = rjd_math_vec4_sub(pp, rjd_math_vec4_setw(s.xyzr, 0));
	return rjd_math_vec4_lengthsq(v) <= powf(rjd_math_vec4_w(s.xyzr), 2);
}

bool rjd_geo_circle_circle(rjd_geo_circle c1, rjd_geo_circle c2) {
	rjd_math_vec3 v = rjd_math_vec3_setz(rjd_math_vec3_sub(c1.xyr, c2.xyr), 0);
	rjd_math_vec3 added = rjd_math_vec3_add(c1.xyr, c2.xyr);
	rjd_math_vec3 squared = rjd_math_vec3_mul(added, added);
	return rjd_math_vec3_lengthsq(v) <= rjd_math_vec3_z(squared);
}

bool rjd_geo_circle_rect(rjd_geo_circle c, rjd_geo_rect r) {
	rjd_math_vec3 min = rjd_math_vec4to3(r.minmax);
	rjd_math_vec3 max = { _mm_movehl_ps(r.minmax.v, r.minmax.v) };

	rjd_math_vec3 center = rjd_math_vec3_setz(c.xyr, 0);
	rjd_math_vec3 p = rjd_math_vec3_max(min, rjd_math_vec3_min(max, center));
	p = rjd_math_vec3_setz(p, 0);
	return rjd_geo_point_circle(p, c);
}

bool rjd_geo_rect_rect(rjd_geo_rect a, rjd_geo_rect b) {
	rjd_math_vec4 min = { _mm_unpacklo_ps(a.minmax.v, b.minmax.v) }; // a.x, b.x, a.y, b.y
	rjd_math_vec4 max = { _mm_unpackhi_ps(a.minmax.v, b.minmax.v) }; // a.x, b.x, a.y, b.y
	max = rjd_math_vec4_shuffle(max,1,0,3,2); // b.x, a.x, b.y, a.y

	return rjd_math_vec4_ge(max, min);
}

bool rjd_geo_sphere_sphere(rjd_geo_sphere a, rjd_geo_sphere b) {
	rjd_math_vec4 v = rjd_math_vec4_setw(rjd_math_vec4_sub(a.xyzr, b.xyzr), 0);
	rjd_math_vec4 squared = rjd_math_vec4_mul(a.xyzr, b.xyzr);
	return rjd_math_vec4_lengthsq(v) <= rjd_math_vec4_w(squared);
}

bool rjd_geo_sphere_box(rjd_geo_sphere s, rjd_geo_box b) {
	rjd_math_vec3 center = rjd_math_vec4to3w(s.xyzr);
	rjd_math_vec3 p = rjd_math_vec3_max(b.min, rjd_math_vec3_min(b.max, center));
	return rjd_geo_point_sphere(p, s);
}

bool rjd_geo_box_box(rjd_geo_box a, rjd_geo_box b) {
	return rjd_math_vec3_ge(a.max, b.min) && rjd_math_vec3_ge(b.max, a.min);
}

bool rjd_geo_ray_point(rjd_geo_ray r, rjd_math_vec3 p, float* t_out) {
	rjd_math_vec3 to_p = rjd_math_vec3_sub(r.p, p);
	rjd_math_vec3 to_p_normalized = rjd_math_vec3_normalize(to_p);
	if (rjd_math_isequal(rjd_math_vec3_dot(to_p_normalized, r.d), 1)) {
		return false;
	}

	if (t_out) {
		*t_out = rjd_math_vec3_length(to_p);
	}

	return true;
}

bool rjd_geo_ray_sphere(rjd_geo_ray r, rjd_geo_sphere s, float* t_out) {
	rjd_math_vec3 center = rjd_math_vec4to3w(s.xyzr);
	rjd_math_vec3 r_to_s = rjd_math_vec3_sub(center, r.p);
	float dot = rjd_math_vec3_dot(r.d, r_to_s);
	if (dot < 0) {
		return false;
	}

	rjd_math_vec3 p = rjd_math_vec3_scale(r.d, dot);
	rjd_math_vec3 s_to_p = rjd_math_vec3_sub(p, center);
	bool colliding = rjd_math_vec3_lengthsq(s_to_p) <= powf(rjd_math_vec4_w(s.xyzr), 2);
	if (colliding && t_out) {
		*t_out = dot;
	}

	return colliding;
}

bool rjd_geo_ray_box(rjd_geo_ray r, rjd_geo_box b, float* t_out) {
	rjd_math_vec3 inv_dir = rjd_math_vec3_div(rjd_math_vec3_one(), r.d);

	float t_dummy = FLT_MAX;
	if (!t_out) {
		t_out = &t_dummy;
	}

	return rjd_geo_ray_boxfast(r.p, inv_dir, b, t_out);
}

bool rjd_geo_ray_boxfast(rjd_math_vec3 ray_pos, rjd_math_vec3 ray_inv_dir, rjd_geo_box b, float* t_out) {
	rjd_math_vec3 v0 = rjd_math_vec3_mul(rjd_math_vec3_sub(b.min, ray_pos), ray_inv_dir);
	rjd_math_vec3 v1 = rjd_math_vec3_mul(rjd_math_vec3_sub(b.max, ray_pos), ray_inv_dir);

	rjd_math_vec3 min = rjd_math_vec3_min(v0, v1);
	rjd_math_vec3 max = rjd_math_vec3_max(v0, v1);

	float tmin = rjd_math_vec3_hmax(min);
	float tmax = rjd_math_vec3_hmin(max);

	bool colliding = (tmax >= 0) && (tmax >= tmin) && (tmin <= *t_out);
	if (colliding) {
		*t_out = tmin;
	}
	return colliding;
}

#endif // RJD_IMPL

