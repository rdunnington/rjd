#define RJD_ENABLE_ASSERT true
#define RJD_ENABLE_LOGGING true
#define RJD_STRBUF_STATIC_SIZE 32

#if _WIN32 || __CYGWIN__
	#define RJD_GFX_BACKEND_D3D11 1
#elif __APPLE__ && __MACH__
	#define RJD_GFX_BACKEND_METAL 1
#endif


#include "rjd_all.h"

