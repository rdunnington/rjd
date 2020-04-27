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

uint32_t rjd_procgeo_rect_calc_num_verts();
uint32_t rjd_procgeo_circle_calc_num_verts(uint32_t tesselation);
uint32_t rjd_procgeo_box_calc_num_verts();
uint32_t rjd_procgeo_cone_calc_num_verts(uint32_t tesselation);
uint32_t rjd_procgeo_cylinder_calc_num_verts(uint32_t tesselation);
uint32_t rjd_procgeo_sphere_calc_num_verts(uint32_t tesselation);

float* rjd_procgeo_rect(float width, float height, float* out, size_t length);
float* rjd_procgeo_circle(float radius, uint32_t tesselation, float* out, size_t length);
float* rjd_procgeo_box(float width, float height, float depth, float* out, size_t length);
float* rjd_procgeo_cone(float height, float radius, uint32_t tesselation, float* out, size_t length);
float* rjd_procgeo_cylinder(float height, float radius, uint32_t tesselation, float* out, size_t length);
float* rjd_procgeo_sphere(float radius, uint32_t tesselation, float* out, size_t length);

////////////////////////////////////////////////////////////////////////////////
// inline implementation

#if RJD_IMPL

const float RJD_PROCGEO_PI = 3.141592653589793238462643f;
const uint32_t RJD_PROCGEO_MIN_TESSELATION_CIRCLE = 3;
const uint32_t RJD_PROCGEO_MIN_TESSELATION_SPHERE = 4;

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

	uint32_t cone = 3 * final_tesselation;
	uint32_t middle = 3 * 2 * final_tesselation * (final_tesselation - 4);

	return cone + middle + cone;
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

	return out + num_verts;
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

	return out + num_verts;
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

	return out;
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

	return out + num_verts;
}

float* rjd_procgeo_cylinder(float height, float radius, uint32_t tesselation, float* out, size_t length)
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

		RJD_ASSERT(top_index <= length);

		uint32_t side_index = (v + v + side_begin_offset) * 3;
		out[side_index++] = cos_1; out[side_index++] = -y; out[side_index++] = sin_1;
		out[side_index++] = cos_1; out[side_index++] =  y; out[side_index++] = sin_1;
		out[side_index++] = cos_2; out[side_index++] =  y; out[side_index++] = sin_2;

		out[side_index++] = cos_2; out[side_index++] =  y; out[side_index++] = sin_2;
		out[side_index++] = cos_2; out[side_index++] = -y; out[side_index++] = sin_2;
		out[side_index++] = cos_1; out[side_index++] = -y; out[side_index++] = sin_1;

		RJD_ASSERT(side_index <= length);
	}

	return out + num_verts;

}


float* rjd_procgeo_sphere(float radius, uint32_t tesselation, float* out, size_t length)
{
	RJD_UNUSED_PARAM(radius);
	RJD_UNUSED_PARAM(tesselation);
	RJD_UNUSED_PARAM(length);

	const uint32_t num_verts = rjd_procgeo_sphere_calc_num_verts(tesselation);
	if (length < num_verts * 3) {
		return NULL;
	}





	return out;
//	const uint32_t num_floats = rjd_procgeo_sphere_calc_num_floats();
//	if (length < ) {
//		return NULL;
//	}
//
//	uint32_t final_tesselation = (tesselation < 4) ? 4 : tesselation;
//	
//	const float r = radius;
//	const float arc_radians = RJD_PROCGEO_PI * 2 / tesselation;
//
//	// top cone
//	
//	
//	// middle
//	
//	// bot cone
//
//	for (uint32_t y_band = 0; y_band < final_tesselation; ++y_band) {
//		for (uint32_t x_band = 0; x_band < final_tesselation; ++y_band) {
//		}
//	}
//	
//	return out;
}

#endif

