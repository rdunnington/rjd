#pragma once

#define RJD_PATH_H 1

//struct rjd_strbuf;
//
//struct rjd_path
//{
//	struct rjd_strbuf buf;
//};

//struct rjd_path rjd_path_init();
//struct rjd_path rjd_path_init_contents(const char* path);
//void rjd_path_append(struct rjd_path* path1, const char* path2);
//void rjd_path_join(struct rjd_path* path1, struct rjd_path path2);
//const char* rjd_path_get(struct rjd_path* path);
//const char* rjd_path_get_extension(struct rjd_path* path);
const char* rjd_path_extension(const char* path);

#if RJD_IMPL

//struct rjd_path rjd_path_init()
//{
//}
//
//struct rjd_path rjd_path_init_contents(const char* initial_contents)
//{
//}
//
//void rjd_path_append(struct rjd_path* path, const char* path)
//{
//}
//
//void rjd_path_join(struct rjd_path* path1, struct rjd_path path2)
//{
//}

const char* rjd_path_extension(const char* path)
{
	if (!path) {
		return NULL;
	}

	const char* extension = strrchr(path, '.');
	// If a path ends with a period, it doesn't have an extension
	if (extension && extension[1] == '\0') {
		return NULL;
	}
	return extension;
}

#endif
