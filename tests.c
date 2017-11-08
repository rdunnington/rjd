#define RJD_IMPL true
#define RJD_ENABLE_ASSERT true
#define RJD_ENABLE_LOGGING true
#define RJD_ENABLE_SHORTNAMES true

#include "rjd_all.h"

char logbuffer[1024 * 128];
size_t logbuffer_pos = 0;
void test_log_hook(const char* msg, size_t length)
{
	UNUSED_PARAM(length);

	strncpy(logbuffer + logbuffer_pos, msg, sizeof(logbuffer) - logbuffer_pos);
}

void test_logging()
{
	struct rjd_logchannel local = {
		.name = "Test Channel",
		.enabled = true,
		.hook = test_log_hook,
	};
	g_rjd_global_logchannel = &local;

	// normal tests
	LOG(

	// disabled

	// verbose

	// other channel

	// expect equals
	
	rjd_log_resetglobal();
}

void test_alloc()
{
	// 
}

void test_array()
{
//	char* stackmem[1024 * 128];
//	memHeap heap = memInitHeap(stackmem, sizeof(stackmem));
//
//	int32* a = rjd_alloc(int32, 8, &heap);
//	rjd_assert(rjd_capacity(a) == 8);
//	rjd_assert(rjd_array_count(a) == 0);
//
//	for (int32 i = 0; i < 8; ++i)
//	{
//		rjd_push(a, i);
//		rjd_assert(a[i] == i);
//		rjd_assert(rjd_array_count(a) == (uint32)i + 1);
//	}
//
//	for (int32 i = 0; rjd_array_count(a) > 0; ++i)
//	{
//		rjd_erase(a, 0);
//		if (rjd_array_count(a) > 0)
//		{
//			rjd_assert(a[0] == i + 1);
//		}
//	}
}

int main(void) 
{
	void test_logging();
	void test_alloc();
	void test_array();

	return 0;
}

