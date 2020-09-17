#define RJD_PROCGEO_H 1

// dependencies:
// * math.h

// Functions for generating procedural geometry:
// * Generates centered at 0,0 scaled by input params. 
// * All functions write 3 floats per vertex, and skip stride number of floats. Pass 0 stride for tightly packed arrays.
// * Returns NULL if there isn't enough space in the float array to generate the geometry. Otherwise, returns the
//   pointer to one past the last element.
// * Use *_calc_num_verts() functions to find how many vertices you need.
// * Vertices are generated in clockwise winding order, assuming view is looking -Z

enum rjd_procgeo_type
{
	RJD_PROCGEO_TYPE_RECT,
	RJD_PROCGEO_TYPE_CIRCLE,
	RJD_PROCGEO_TYPE_BOX,
	RJD_PROCGEO_TYPE_CONE,
	RJD_PROCGEO_TYPE_CYLINDER,
	RJD_PROCGEO_TYPE_SPHERE,
	RJD_PROCGEO_TYPE_COUNT,
};

uint32_t rjd_procgeo_calc_num_verts(enum rjd_procgeo_type type, uint32_t tesselation);
float* rjd_procgeo(enum rjd_procgeo_type type, uint32_t tesselation, float size_x, float size_y, float size_z, float* out, uint32_t length, uint32_t stride);

uint32_t rjd_procgeo_rect_calc_num_verts(void);
uint32_t rjd_procgeo_circle_calc_num_verts(uint32_t tesselation);
uint32_t rjd_procgeo_box_calc_num_verts(void);
uint32_t rjd_procgeo_cone_calc_num_verts(uint32_t tesselation);
uint32_t rjd_procgeo_cylinder_calc_num_verts(uint32_t tesselation);
uint32_t rjd_procgeo_sphere_calc_num_verts(uint32_t tesselation);

float* rjd_procgeo_rect(float width, float height, float* out, uint32_t length, uint32_t stride);
float* rjd_procgeo_circle(float radius, uint32_t tesselation, float* out, uint32_t length, uint32_t stride);
float* rjd_procgeo_box(float width, float height, float depth, float* out, uint32_t length, uint32_t stride);
float* rjd_procgeo_cone(float radius, float height, uint32_t tesselation, float* out, uint32_t length, uint32_t stride);
float* rjd_procgeo_cylinder(float radius, float height, uint32_t tesselation, float* out, uint32_t length, uint32_t stride);
float* rjd_procgeo_sphere(float radius, uint32_t tesselation, float* out, uint32_t length, uint32_t stride);

////////////////////////////////////////////////////////////////////////////////
// inline implementation

#if RJD_IMPL

const float RJD_PROCGEO_PI = 3.141592653589793238462643f;
const uint32_t RJD_PROCGEO_MIN_TESSELATION_CIRCLE = 3;
const uint32_t RJD_PROCGEO_MIN_TESSELATION_SPHERE = 3;
const uint32_t RJD_PROCGEO_FLOATS_PER_TRI = 3;

uint32_t rjd_procgeo_calc_num_verts(enum rjd_procgeo_type type, uint32_t tesselation)
{
	switch (type)
	{
	 	case RJD_PROCGEO_TYPE_RECT: return rjd_procgeo_rect_calc_num_verts();
	 	case RJD_PROCGEO_TYPE_CIRCLE: return rjd_procgeo_circle_calc_num_verts(tesselation);
	 	case RJD_PROCGEO_TYPE_BOX: return rjd_procgeo_box_calc_num_verts();
	 	case RJD_PROCGEO_TYPE_CONE: return rjd_procgeo_cone_calc_num_verts(tesselation);
	 	case RJD_PROCGEO_TYPE_CYLINDER: return rjd_procgeo_cylinder_calc_num_verts(tesselation);
	 	case RJD_PROCGEO_TYPE_SPHERE: return rjd_procgeo_sphere_calc_num_verts(tesselation);
		default: break;
	}
	RJD_ASSERTFAIL("Unknown type: %d", type);
	return 0;
}

float* rjd_procgeo(enum rjd_procgeo_type type, uint32_t tesselation, float size_x, float size_y, float size_z, float* out, uint32_t length, uint32_t stride)
{
	switch (type)
	{
	 	case RJD_PROCGEO_TYPE_RECT: return rjd_procgeo_rect(size_x, size_y, out, length, stride);
	 	case RJD_PROCGEO_TYPE_CIRCLE: return rjd_procgeo_circle(size_x, tesselation, out, length, stride);
	 	case RJD_PROCGEO_TYPE_BOX: return rjd_procgeo_box(size_x, size_y, size_z, out, length, stride);
	 	case RJD_PROCGEO_TYPE_CONE: return rjd_procgeo_cone(size_x, size_y, tesselation, out, length, stride);
	 	case RJD_PROCGEO_TYPE_CYLINDER: return rjd_procgeo_cylinder(size_x, size_y, tesselation, out, length, stride);
	 	case RJD_PROCGEO_TYPE_SPHERE: return rjd_procgeo_sphere(size_x, tesselation, out, length, stride);
		default: break;
	}

	RJD_ASSERTFAIL("Unknown type: %d", type);
	return out;
}

uint32_t rjd_procgeo_rect_calc_num_verts() {
	return RJD_PROCGEO_FLOATS_PER_TRI * 2;
}

uint32_t rjd_procgeo_circle_calc_num_verts(uint32_t tesselation) {
	uint32_t final_tesselation = RJD_PROCGEO_MIN_TESSELATION_CIRCLE + tesselation;
	return RJD_PROCGEO_FLOATS_PER_TRI * final_tesselation;
}

uint32_t rjd_procgeo_box_calc_num_verts() {
	return RJD_PROCGEO_FLOATS_PER_TRI * 2 * 6;
}

uint32_t rjd_procgeo_cone_calc_num_verts(uint32_t tesselation) {
	return 2 * rjd_procgeo_circle_calc_num_verts(tesselation);
}

uint32_t rjd_procgeo_cylinder_calc_num_verts(uint32_t tesselation)
{
	uint32_t circle_verts = rjd_procgeo_circle_calc_num_verts(tesselation);
	uint32_t final_tesselation = RJD_PROCGEO_MIN_TESSELATION_CIRCLE + tesselation;
	uint32_t quad_verts = RJD_PROCGEO_FLOATS_PER_TRI * 2 * final_tesselation;
	return quad_verts + (circle_verts * 2);
}

uint32_t rjd_procgeo_sphere_calc_num_verts(uint32_t tesselation) {
	uint32_t final_tesselation = RJD_PROCGEO_MIN_TESSELATION_SPHERE + tesselation;
	uint32_t tri_verts = RJD_PROCGEO_FLOATS_PER_TRI;
	uint32_t quad_verts = tri_verts * 2;

	return quad_verts * final_tesselation * final_tesselation - tri_verts * final_tesselation * 2;
}

float* rjd_procgeo_rect(float width, float height, float* out, uint32_t length, uint32_t stride)
{
	const uint32_t floats_per_vert = RJD_PROCGEO_FLOATS_PER_TRI + (uint32_t)stride;
	const uint32_t num_verts = rjd_procgeo_rect_calc_num_verts();
	if (length < num_verts * floats_per_vert) {
		return NULL;
	}

	const float x = width / 2.0f;
	const float y = height / 2.0f;

	int32_t i = 0;

	out[i++] = -x; out[i++] = -y; out[i++] = 0.0f; i += stride;
	out[i++] =  x; out[i++] =  y; out[i++] = 0.0f; i += stride;
	out[i++] = -x; out[i++] =  y; out[i++] = 0.0f; i += stride;

	out[i++] = -x; out[i++] = -y; out[i++] = 0.0f; i += stride;
	out[i++] =  x; out[i++] = -y; out[i++] = 0.0f; i += stride;
	out[i++] =  x; out[i++] =  y; out[i++] = 0.0f; i += stride;

	return out + i;
}

float* rjd_procgeo_circle(float radius, uint32_t tesselation, float* out, uint32_t length, uint32_t stride)
{
	const uint32_t floats_per_vert = RJD_PROCGEO_FLOATS_PER_TRI + stride;
	const uint32_t num_verts = rjd_procgeo_circle_calc_num_verts(tesselation);
	if (length < num_verts * floats_per_vert) {
		return NULL;
	}

	const uint32_t final_tesselation = tesselation + RJD_PROCGEO_MIN_TESSELATION_CIRCLE;
	const float arc_radians = RJD_PROCGEO_PI * 2 / final_tesselation;

	for (uint32_t i = 0, arc_segment = 0; i < num_verts * floats_per_vert; ++arc_segment) {
		float p1_radians = arc_radians * arc_segment;
		float p2_radians = arc_radians * (arc_segment + 1);

		out[i++] = 0;							out[i++] = 0;							out[i++] = 0; i += stride;
		out[i++] = cosf(p1_radians) * radius;	out[i++] = sinf(p1_radians) * radius;	out[i++] = 0; i += stride;
		out[i++] = cosf(p2_radians) * radius;	out[i++] = sinf(p2_radians) * radius;	out[i++] = 0; i += stride;
	}

	return out + num_verts * floats_per_vert;
}

float* rjd_procgeo_box(float width, float height, float depth, float* out, uint32_t length, uint32_t stride)
{
	const uint32_t floats_per_vert = RJD_PROCGEO_FLOATS_PER_TRI + stride;
	const uint32_t num_verts = rjd_procgeo_box_calc_num_verts();
	if (length < num_verts * floats_per_vert) {
		return NULL;
	}

	const float x = width / 2;
	const float y = height / 2;
	const float z = depth / 2;

	int i = 0;

	// front
	out[i++] = -x; out[i++] =  y; out[i++] =  z; i += stride;
	out[i++] = -x; out[i++] = -y; out[i++] =  z; i += stride;
	out[i++] =  x; out[i++] = -y; out[i++] =  z; i += stride;
	out[i++] = -x; out[i++] =  y; out[i++] =  z; i += stride;
	out[i++] =  x; out[i++] = -y; out[i++] =  z; i += stride;
	out[i++] =  x; out[i++] =  y; out[i++] =  z; i += stride;

	// back
	out[i++] = -x; out[i++] =  y; out[i++] = -z; i += stride;
	out[i++] =  x; out[i++] = -y; out[i++] = -z; i += stride;
	out[i++] = -x; out[i++] = -y; out[i++] = -z; i += stride;
	out[i++] = -x; out[i++] =  y; out[i++] = -z; i += stride;
	out[i++] =  x; out[i++] =  y; out[i++] = -z; i += stride;
	out[i++] =  x; out[i++] = -y; out[i++] = -z; i += stride;

	// left
	out[i++] = -x; out[i++] = -y; out[i++] = -z; i += stride;
	out[i++] = -x; out[i++] = -y; out[i++] =  z; i += stride;
	out[i++] = -x; out[i++] =  y; out[i++] =  z; i += stride;
	out[i++] = -x; out[i++] = -y; out[i++] = -z; i += stride;
	out[i++] = -x; out[i++] =  y; out[i++] =  z; i += stride;
	out[i++] = -x; out[i++] =  y; out[i++] = -z; i += stride;

	// right
	out[i++] =  x; out[i++] = -y; out[i++] = -z; i += stride;
	out[i++] =  x; out[i++] =  y; out[i++] = -z; i += stride;
	out[i++] =  x; out[i++] =  y; out[i++] =  z; i += stride;
	out[i++] =  x; out[i++] = -y; out[i++] = -z; i += stride;
	out[i++] =  x; out[i++] =  y; out[i++] =  z; i += stride;
	out[i++] =  x; out[i++] = -y; out[i++] =  z; i += stride;

	// top
	out[i++] =  x; out[i++] =  y; out[i++] = -z; i += stride;
	out[i++] = -x; out[i++] =  y; out[i++] = -z; i += stride;
	out[i++] = -x; out[i++] =  y; out[i++] =  z; i += stride;
	out[i++] =  x; out[i++] =  y; out[i++] = -z; i += stride;
	out[i++] = -x; out[i++] =  y; out[i++] =  z; i += stride;
	out[i++] =  x; out[i++] =  y; out[i++] =  z; i += stride;

	// bottom
	out[i++] = -x; out[i++] = -y; out[i++] =  z; i += stride;
	out[i++] = -x; out[i++] = -y; out[i++] = -z; i += stride;
	out[i++] =  x; out[i++] = -y; out[i++] = -z; i += stride;
	out[i++] = -x; out[i++] = -y; out[i++] =  z; i += stride;
	out[i++] =  x; out[i++] = -y; out[i++] = -z; i += stride;
	out[i++] =  x; out[i++] = -y; out[i++] =  z; i += stride;

	return out + i;
}

float* rjd_procgeo_cone(float height, float radius, uint32_t tesselation, float* out, uint32_t length, uint32_t stride)
{
	const uint32_t floats_per_vert = RJD_PROCGEO_FLOATS_PER_TRI + stride;
	const uint32_t num_verts = rjd_procgeo_cone_calc_num_verts(tesselation);
	if (length < num_verts * floats_per_vert) {
		return NULL;
	}

	const uint32_t final_tesselation = tesselation + RJD_PROCGEO_MIN_TESSELATION_CIRCLE;
	const float arc_radians = RJD_PROCGEO_PI * 2 / final_tesselation;

	const uint32_t top_begin_offset = (num_verts / 2) * floats_per_vert;
	const float cone_y = height / 2;

	for (uint32_t i = 0, arc_segment = 0; i < top_begin_offset; i += 3 * (floats_per_vert), ++arc_segment) {
		float p1_radians = arc_radians * arc_segment;
		float p2_radians = arc_radians * (arc_segment + 1);

		float cos_1 = cosf(p1_radians) * radius; 
		float sin_1 = sinf(p1_radians) * radius;
		float cos_2 = cosf(p2_radians) * radius; 
		float sin_2 = sinf(p2_radians) * radius;

		uint32_t bot_index = i;
		uint32_t top_index = i + top_begin_offset;

		out[bot_index++] = 0;		out[bot_index++] = -cone_y;	out[bot_index++] = 0;		bot_index += stride;
		out[bot_index++] = cos_1;	out[bot_index++] = -cone_y;	out[bot_index++] = sin_1;	bot_index += stride;
		out[bot_index++] = cos_2;	out[bot_index++] = -cone_y;	out[bot_index++] = sin_2;	bot_index += stride;

		out[top_index++] = 0;		out[top_index++] = cone_y;	out[top_index++] = 0;		top_index += stride;
		out[top_index++] = cos_2;	out[top_index++] = -cone_y;	out[top_index++] = sin_2;	top_index += stride;
		out[top_index++] = cos_1;	out[top_index++] = -cone_y;	out[top_index++] = sin_1;	top_index += stride;
	}

	return out + num_verts * floats_per_vert;
}

float* rjd_procgeo_cylinder(float radius, float height, uint32_t tesselation, float* out, uint32_t length, uint32_t stride)
{
	const uint32_t floats_per_vert = RJD_PROCGEO_FLOATS_PER_TRI + stride;
	const uint32_t num_verts = rjd_procgeo_cylinder_calc_num_verts(tesselation);
	if (length < num_verts * floats_per_vert) {
		return NULL;
	}

	const uint32_t final_tesselation = tesselation + RJD_PROCGEO_MIN_TESSELATION_CIRCLE;
	const float arc_radians = RJD_PROCGEO_PI * 2 / final_tesselation;

	const uint32_t top_begin_offset = rjd_procgeo_circle_calc_num_verts(tesselation);
	const uint32_t side_begin_offset = top_begin_offset * 2;
	const float y = height / 2;

	for (uint32_t v = 0, arc_segment = 0; v < top_begin_offset; v += RJD_PROCGEO_FLOATS_PER_TRI, ++arc_segment) {
		float p1_radians = arc_radians * arc_segment;
		float p2_radians = arc_radians * (arc_segment + 1);

		float cos_1 = cosf(p1_radians) * radius; 
		float sin_1 = sinf(p1_radians) * radius;
		float cos_2 = cosf(p2_radians) * radius; 
		float sin_2 = sinf(p2_radians) * radius;

		uint32_t bot_index = v * floats_per_vert;
		out[bot_index++] = 0;		out[bot_index++] = -y; out[bot_index++] = 0;		bot_index += stride;
		out[bot_index++] = cos_1;	out[bot_index++] = -y; out[bot_index++] = sin_1;	bot_index += stride;
		out[bot_index++] = cos_2;	out[bot_index++] = -y; out[bot_index++] = sin_2;	bot_index += stride;

		uint32_t top_index = (v + top_begin_offset) * floats_per_vert;
		out[top_index++] = 0;		out[top_index++] = y; out[top_index++] = 0;		top_index += stride;
		out[top_index++] = cos_2;	out[top_index++] = y; out[top_index++] = sin_2;	top_index += stride;
		out[top_index++] = cos_1;	out[top_index++] = y; out[top_index++] = sin_1;	top_index += stride;

		uint32_t side_index = (v + v + side_begin_offset) * floats_per_vert;
		out[side_index++] = cos_1; out[side_index++] = -y; out[side_index++] = sin_1; side_index += stride;
		out[side_index++] = cos_1; out[side_index++] =  y; out[side_index++] = sin_1; side_index += stride;
		out[side_index++] = cos_2; out[side_index++] =  y; out[side_index++] = sin_2; side_index += stride;

		out[side_index++] = cos_2; out[side_index++] =  y; out[side_index++] = sin_2; side_index += stride;
		out[side_index++] = cos_2; out[side_index++] = -y; out[side_index++] = sin_2; side_index += stride;
		out[side_index++] = cos_1; out[side_index++] = -y; out[side_index++] = sin_1; side_index += stride;
	}

	return out + num_verts * floats_per_vert;
}

float* rjd_procgeo_sphere(float radius, uint32_t tesselation, float* out, uint32_t length, uint32_t stride)
{
	const uint32_t floats_per_vert = RJD_PROCGEO_FLOATS_PER_TRI + stride;
	const uint32_t num_verts = rjd_procgeo_sphere_calc_num_verts(tesselation);
	if (length < num_verts * floats_per_vert) {
		return NULL;
	}
	
	const float pi = RJD_PROCGEO_PI;

	const uint32_t final_tesselation = tesselation + RJD_PROCGEO_MIN_TESSELATION_SPHERE;

	uint32_t i = 0;

	for (uint32_t y_arc = 0; y_arc < final_tesselation; ++y_arc) {

		float y_arc1 = (float)y_arc / final_tesselation;
		float y_arc2 = (float)(y_arc + 1) / final_tesselation;

		float cos_2pi_y_arc1 = cosf(2 * pi * y_arc1);
		float cos_2pi_y_arc2 = cosf(2 * pi * y_arc2);

		float sin_2pi_y_arc1 = sinf(2 * pi * y_arc1);
		float sin_2pi_y_arc2 = sinf(2 * pi * y_arc2);

		for (uint32_t xz_arc = 0; xz_arc < final_tesselation; ++xz_arc) {

			float xz_arc1 = (float)xz_arc / final_tesselation;
			float xz_arc2 = (float)(xz_arc + 1) / final_tesselation;

			float sin_pi_xz_arc1 = sinf(pi * xz_arc1);
			float cos_pi_xz_arc1 = cosf(pi * xz_arc1);

			float sin_pi_xz_arc2 = sinf(pi * xz_arc2);
			float cos_pi_xz_arc2 = cosf(pi * xz_arc2);

			float x1 = sin_pi_xz_arc1 * cos_2pi_y_arc1 * radius;
			float y1 = cos_pi_xz_arc1 * radius;
			float z1 = sin_pi_xz_arc1 * sin_2pi_y_arc1 * radius;

			float x2 = sin_pi_xz_arc2 * cos_2pi_y_arc1 * radius;
			float y2 = cos_pi_xz_arc2 * radius;
			float z2 = sin_pi_xz_arc2 * sin_2pi_y_arc1 * radius;

			float x3 = sin_pi_xz_arc1 * cos_2pi_y_arc2 * radius;
			float y3 = cos_pi_xz_arc1 * radius;
			float z3 = sin_pi_xz_arc1 * sin_2pi_y_arc2 * radius;

			float x4 = sin_pi_xz_arc2 * cos_2pi_y_arc2 * radius;
			float y4 = cos_pi_xz_arc2 * radius;
			float z4 = sin_pi_xz_arc2 * sin_2pi_y_arc2 * radius;

			if (xz_arc > 0)
			{
				out[i++] = x1; out[i++] = y1; out[i++] = z1; i += stride;
				out[i++] = x3; out[i++] = y3; out[i++] = z3; i += stride;
				out[i++] = x2; out[i++] = y2; out[i++] = z2; i += stride;
			}

			if (xz_arc < final_tesselation - 1)
			{
				out[i++] = x2; out[i++] = y2; out[i++] = z2; i += stride;
				out[i++] = x3; out[i++] = y3; out[i++] = z3; i += stride;
				out[i++] = x4; out[i++] = y4; out[i++] = z4; i += stride;
			}
		}
	}

	return out + num_verts * floats_per_vert;
}

#endif

