#define RJD_PROCGEO_H 1

// dependencies:
// * math.h

// Functions for generating procedural geometry:
// * Generates triangles in normalized [0,1] space centered at 0,0. 
// * All functions write 3 floats per vertex.
// * Returns NULL if there isn't enough space in the float array to generate the geometry
// * Returns the pointer to one-past the last element written
// * Use *_calc_num_verts() functions to find how many floats you need
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
float* rjd_procgeo(enum rjd_procgeo_type type, uint32_t tesselation, float size_x, float size_y, float size_z, float* out, size_t length);

uint32_t rjd_procgeo_rect_calc_num_verts(void);
uint32_t rjd_procgeo_circle_calc_num_verts(uint32_t tesselation);
uint32_t rjd_procgeo_box_calc_num_verts(void);
uint32_t rjd_procgeo_cone_calc_num_verts(uint32_t tesselation);
uint32_t rjd_procgeo_cylinder_calc_num_verts(uint32_t tesselation);
uint32_t rjd_procgeo_sphere_calc_num_verts(uint32_t tesselation);

float* rjd_procgeo_rect(float width, float height, float* out, size_t length);
float* rjd_procgeo_circle(float radius, uint32_t tesselation, float* out, size_t length);
float* rjd_procgeo_box(float width, float height, float depth, float* out, size_t length);
float* rjd_procgeo_cone(float radius, float height, uint32_t tesselation, float* out, size_t length);
float* rjd_procgeo_cylinder(float radius, float height, uint32_t tesselation, float* out, size_t length);
float* rjd_procgeo_sphere(float radius, uint32_t tesselation, float* out, size_t length);

////////////////////////////////////////////////////////////////////////////////
// inline implementation

#if RJD_IMPL

const float RJD_PROCGEO_PI = 3.141592653589793238462643f;
const uint32_t RJD_PROCGEO_MIN_TESSELATION_CIRCLE = 3;
const uint32_t RJD_PROCGEO_MIN_TESSELATION_SPHERE = 3;

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

float* rjd_procgeo(enum rjd_procgeo_type type, uint32_t tesselation, float size_x, float size_y, float size_z, float* out, size_t length)
{
	switch (type)
	{
	 	case RJD_PROCGEO_TYPE_RECT: return rjd_procgeo_rect(size_x, size_y, out, length);
	 	case RJD_PROCGEO_TYPE_CIRCLE: return rjd_procgeo_circle(size_x, tesselation, out, length);
	 	case RJD_PROCGEO_TYPE_BOX: return rjd_procgeo_box(size_x, size_y, size_z, out, length);
	 	case RJD_PROCGEO_TYPE_CONE: return rjd_procgeo_cone(size_x, size_y, tesselation, out, length);
	 	case RJD_PROCGEO_TYPE_CYLINDER: return rjd_procgeo_cylinder(size_x, size_y, tesselation, out, length);
	 	case RJD_PROCGEO_TYPE_SPHERE: return rjd_procgeo_sphere(size_x, tesselation, out, length);
		default: break;
	}

	RJD_ASSERTFAIL("Unknown type: %d", type);
	return out;
}

uint32_t rjd_procgeo_rect_calc_num_verts() {
	// 3 verts per triangle, 2 triangles
	return 3 * 2;
}

uint32_t rjd_procgeo_circle_calc_num_verts(uint32_t tesselation) {
	uint32_t final_tesselation = RJD_PROCGEO_MIN_TESSELATION_CIRCLE + tesselation;
	return 3 * final_tesselation;
}

uint32_t rjd_procgeo_box_calc_num_verts() {
	return 3 * 2 * 6;
}

uint32_t rjd_procgeo_cone_calc_num_verts(uint32_t tesselation) {
	return 2 * rjd_procgeo_circle_calc_num_verts(tesselation);
}

uint32_t rjd_procgeo_cylinder_calc_num_verts(uint32_t tesselation)
{
	uint32_t circle_verts = rjd_procgeo_circle_calc_num_verts(tesselation);
	uint32_t final_tesselation = RJD_PROCGEO_MIN_TESSELATION_CIRCLE + tesselation;
	uint32_t quad_verts = 3 * 2 * final_tesselation;
	return quad_verts + (circle_verts * 2);
}

uint32_t rjd_procgeo_sphere_calc_num_verts(uint32_t tesselation) {
	uint32_t final_tesselation = RJD_PROCGEO_MIN_TESSELATION_SPHERE + tesselation;
	uint32_t tri_verts = 3;
	uint32_t quad_verts = tri_verts * 2;

	return quad_verts * final_tesselation * final_tesselation - tri_verts * final_tesselation * 2;
}

float* rjd_procgeo_rect(float width, float height, float* out, size_t length)
{
	const uint32_t num_verts = rjd_procgeo_rect_calc_num_verts();
	if (length < num_verts * 3) {
		return NULL;
	}

	const float x = width / 2.0f;
	const float y = height / 2.0f;

	int32_t i = 0;

	out[i++] = -x; out[i++] = -y; out[i++] = 0.0f;
	out[i++] =  x; out[i++] =  y; out[i++] = 0.0f;
	out[i++] = -x; out[i++] =  y; out[i++] = 0.0f;

	out[i++] = -x; out[i++] = -y; out[i++] = 0.0f;
	out[i++] =  x; out[i++] = -y; out[i++] = 0.0f;
	out[i++] =  x; out[i++] =  y; out[i++] = 0.0f;

	return out + num_verts * 3;
}

float* rjd_procgeo_circle(float radius, uint32_t tesselation, float* out, size_t length)
{
	const uint32_t num_verts = rjd_procgeo_circle_calc_num_verts(tesselation);
	if (length < num_verts * 3) {
		return NULL;
	}

	const uint32_t final_tesselation = tesselation + RJD_PROCGEO_MIN_TESSELATION_CIRCLE;
	const float arc_radians = RJD_PROCGEO_PI * 2 / final_tesselation;

	for (uint32_t i = 0, arc_segment = 0; i < num_verts * 3; i += 3 * 3, ++arc_segment) {
		out[i + 0] = 0;
		out[i + 1] = 0;
		out[i + 2] = 0;
		float p1_radians = arc_radians * arc_segment;
		out[i + 3] = cos(p1_radians) * radius;
		out[i + 4] = sin(p1_radians) * radius;
		out[i + 5] = 0;
		float p2_radians = arc_radians * (arc_segment + 1);
		out[i + 6] = cos(p2_radians) * radius;
		out[i + 7] = sin(p2_radians) * radius;
		out[i + 8] = 0;
	}

	return out + num_verts * 3;
}

float* rjd_procgeo_box(float width, float height, float depth, float* out, size_t length)
{
	const uint32_t num_verts = rjd_procgeo_box_calc_num_verts();
	if (length < num_verts * 3) {
		return NULL;
	}

	const float x = width / 2;
	const float y = height / 2;
	const float z = depth / 2;

	int i = 0;

	// front
	out[i++] = -x; out[i++] =  y; out[i++] =  z;
	out[i++] = -x; out[i++] = -y; out[i++] =  z;
	out[i++] =  x; out[i++] = -y; out[i++] =  z;
	out[i++] = -x; out[i++] =  y; out[i++] =  z;
	out[i++] =  x; out[i++] = -y; out[i++] =  z;
	out[i++] =  x; out[i++] =  y; out[i++] =  z;

	// back
	out[i++] = -x; out[i++] =  y; out[i++] = -z;
	out[i++] =  x; out[i++] = -y; out[i++] = -z;
	out[i++] = -x; out[i++] = -y; out[i++] = -z;
	out[i++] = -x; out[i++] =  y; out[i++] = -z;
	out[i++] =  x; out[i++] =  y; out[i++] = -z;
	out[i++] =  x; out[i++] = -y; out[i++] = -z;

	// left
	out[i++] = -x; out[i++] = -y; out[i++] = -z;
	out[i++] = -x; out[i++] = -y; out[i++] =  z;
	out[i++] = -x; out[i++] =  y; out[i++] =  z;
	out[i++] = -x; out[i++] = -y; out[i++] = -z;
	out[i++] = -x; out[i++] =  y; out[i++] =  z;
	out[i++] = -x; out[i++] =  y; out[i++] = -z;

	// right
	out[i++] =  x; out[i++] = -y; out[i++] = -z; 
	out[i++] =  x; out[i++] =  y; out[i++] = -z; 
	out[i++] =  x; out[i++] =  y; out[i++] =  z; 
	out[i++] =  x; out[i++] = -y; out[i++] = -z; 
	out[i++] =  x; out[i++] =  y; out[i++] =  z; 
	out[i++] =  x; out[i++] = -y; out[i++] =  z; 

	// top
	out[i++] =  x; out[i++] =  y; out[i++] = -z; 
	out[i++] = -x; out[i++] =  y; out[i++] = -z; 
	out[i++] = -x; out[i++] =  y; out[i++] =  z; 
	out[i++] =  x; out[i++] =  y; out[i++] = -z; 
	out[i++] = -x; out[i++] =  y; out[i++] =  z; 
	out[i++] =  x; out[i++] =  y; out[i++] =  z; 

	// bottom
	out[i++] = -x; out[i++] = -y; out[i++] =  z; 
	out[i++] = -x; out[i++] = -y; out[i++] = -z; 
	out[i++] =  x; out[i++] = -y; out[i++] = -z; 
	out[i++] = -x; out[i++] = -y; out[i++] =  z; 
	out[i++] =  x; out[i++] = -y; out[i++] = -z; 
	out[i++] =  x; out[i++] = -y; out[i++] =  z; 

	return out + num_verts * 3;
}

float* rjd_procgeo_cone(float height, float radius, uint32_t tesselation, float* out, size_t length)
{
	const uint32_t num_verts = rjd_procgeo_cone_calc_num_verts(tesselation);
	if (length < num_verts * 3) {
		return NULL;
	}

	const uint32_t final_tesselation = tesselation + RJD_PROCGEO_MIN_TESSELATION_CIRCLE;
	const float arc_radians = RJD_PROCGEO_PI * 2 / final_tesselation;

	const uint32_t top_begin_offset = (num_verts / 2) * 3;
	const float cone_y = height / 2;

	for (uint32_t i = 0, arc_segment = 0; i < top_begin_offset; i += 3 * 3, ++arc_segment) {
		float p1_radians = arc_radians * arc_segment;
		float p2_radians = arc_radians * (arc_segment + 1);

		float cos_1 = cos(p1_radians) * radius; 
		float sin_1 = sin(p1_radians) * radius;
		float cos_2 = cos(p2_radians) * radius; 
		float sin_2 = sin(p2_radians) * radius;

		uint32_t bot_index = i;
		out[bot_index++] = 0;
		out[bot_index++] = -cone_y;
		out[bot_index++] = 0;
		out[bot_index++] = cos_1;
		out[bot_index++] = -cone_y;
		out[bot_index++] = sin_1;
		out[bot_index++] = cos_2;
		out[bot_index++] = -cone_y;
		out[bot_index++] = sin_2;

		uint32_t top_index = i + top_begin_offset;
		out[top_index++] = 0;
		out[top_index++] = cone_y;
		out[top_index++] = 0;
		out[top_index++] = cos_2;
		out[top_index++] = -cone_y;
		out[top_index++] = sin_2;
		out[top_index++] = cos_1;
		out[top_index++] = -cone_y;
		out[top_index++] = sin_1;
	}

	return out + num_verts * 3;
}

float* rjd_procgeo_cylinder(float radius, float height, uint32_t tesselation, float* out, size_t length)
{
	const uint32_t num_verts = rjd_procgeo_cylinder_calc_num_verts(tesselation);
	if (length < num_verts * 3) {
		return NULL;
	}

	const uint32_t final_tesselation = tesselation + RJD_PROCGEO_MIN_TESSELATION_CIRCLE;
	const float arc_radians = RJD_PROCGEO_PI * 2 / final_tesselation;

	const uint32_t top_begin_offset = rjd_procgeo_circle_calc_num_verts(tesselation);
	const uint32_t side_begin_offset = top_begin_offset * 2;
	const float y = height / 2;

	for (uint32_t v = 0, arc_segment = 0; v < top_begin_offset; v += 3, ++arc_segment) {
		float p1_radians = arc_radians * arc_segment;
		float p2_radians = arc_radians * (arc_segment + 1);

		float cos_1 = cos(p1_radians) * radius; 
		float sin_1 = sin(p1_radians) * radius;
		float cos_2 = cos(p2_radians) * radius; 
		float sin_2 = sin(p2_radians) * radius;

		uint32_t i = v * 3;

		uint32_t bot_index = i;
		out[bot_index++] = 0;
		out[bot_index++] = -y;
		out[bot_index++] = 0;
		out[bot_index++] = cos_1;
		out[bot_index++] = -y;
		out[bot_index++] = sin_1;
		out[bot_index++] = cos_2;
		out[bot_index++] = -y;
		out[bot_index++] = sin_2;

		uint32_t top_index = (v + top_begin_offset) * 3;
		out[top_index++] = 0;
		out[top_index++] = y;
		out[top_index++] = 0;
		out[top_index++] = cos_2;
		out[top_index++] = y;
		out[top_index++] = sin_2;
		out[top_index++] = cos_1;
		out[top_index++] = y;
		out[top_index++] = sin_1;

		uint32_t side_index = (v + v + side_begin_offset) * 3;
		out[side_index++] = cos_1; out[side_index++] = -y; out[side_index++] = sin_1;
		out[side_index++] = cos_1; out[side_index++] =  y; out[side_index++] = sin_1;
		out[side_index++] = cos_2; out[side_index++] =  y; out[side_index++] = sin_2;

		out[side_index++] = cos_2; out[side_index++] =  y; out[side_index++] = sin_2;
		out[side_index++] = cos_2; out[side_index++] = -y; out[side_index++] = sin_2;
		out[side_index++] = cos_1; out[side_index++] = -y; out[side_index++] = sin_1;
	}

	return out + num_verts * 3;
}

float* rjd_procgeo_sphere(float radius, uint32_t tesselation, float* out, size_t length)
{
	const uint32_t num_verts = rjd_procgeo_sphere_calc_num_verts(tesselation);
	if (length < num_verts * 3) {
		return NULL;
	}
	
	const float pi = RJD_PROCGEO_PI;

	const uint32_t final_tesselation = tesselation + RJD_PROCGEO_MIN_TESSELATION_SPHERE;

	uint32_t i = 0;

	for (uint32_t y_arc = 0; y_arc < final_tesselation; ++y_arc) {

		float y_arc1 = (float)y_arc / final_tesselation;
		float y_arc2 = (float)(y_arc + 1) / final_tesselation;

		float cos_2pi_y_arc1 = cos(2 * pi * y_arc1);
		float cos_2pi_y_arc2 = cos(2 * pi * y_arc2);

		float sin_2pi_y_arc1 = sin(2 * pi * y_arc1);
		float sin_2pi_y_arc2 = sin(2 * pi * y_arc2);

		for (uint32_t xz_arc = 0; xz_arc < final_tesselation; ++xz_arc) {

			float xz_arc1 = (float)xz_arc / final_tesselation;
			float xz_arc2 = (float)(xz_arc + 1) / final_tesselation;

			float sin_pi_xz_arc1 = sin(pi * xz_arc1);
			float cos_pi_xz_arc1 = cos(pi * xz_arc1);

			float sin_pi_xz_arc2 = sin(pi * xz_arc2);
			float cos_pi_xz_arc2 = cos(pi * xz_arc2);

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
				out[i++] = x1; out[i++] = y1; out[i++] = z1;
				out[i++] = x3; out[i++] = y3; out[i++] = z3;
				out[i++] = x2; out[i++] = y2; out[i++] = z2;
			}

			if (xz_arc < final_tesselation - 1)
			{
				out[i++] = x2; out[i++] = y2; out[i++] = z2;
				out[i++] = x3; out[i++] = y3; out[i++] = z3;
				out[i++] = x4; out[i++] = y4; out[i++] = z4;
			}
		}
	}

	return out + num_verts * 3;
}

#endif

