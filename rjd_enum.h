#pragma once

#define RJD_ENUM 1

// TODO support RJD_ENABLE_ENUM_TYPEDEF

// To use these macros, you need to define an xmacro list, then the use the appropriate *DECLARE
// in a header and *DEFINE in source files. By using these macros, you get:
// * The enum definition (as if you defined it yourself)
// * A count of members in the enum named k_<enumname>_count
// * An enumname_tostring() function that takes the enum and returns a static string (no mem allocation)
// * An enumname_parse() function that takes a const char* and pointer to enum and returns success/fail
// * An array of the enum values' string representation
//
// For example, let's define a Result enum with the values Success and Fail. Note to not forget the 
// backslash for extending the macro in the real version.
//
//	#define MY_ENUM_LIST(macro)
//		macro(RESULT_FAIL)
//		macro(RESULT_SUCCESS)
//	RJD_ENUM_DECLARE(Result, MY_ENUM_LIST)
//	RJD_ENUM_DEFINE(Result, MY_ENUM_LIST)
//
//	The generated interface for the Result enum above would be:
//		enum Result { RESULT_FAIL, RESULT_SUCCESS };
//		enum { k_Result_count = 2 };
//		const char* Result_tostring(enum Result v);
//		bool Result_parse(const char* s, enum Result v);
//		const char* s_Result_strings[] = { "RESULT_FAIL", "RESULT_SUCCESS" };
//
//	You can also specify custom strings if you want to override the default tostring/parse:
//
//	#define MY_ENUM_LIST2(macro)
//		macro(MY_ENUM_LIST2_V1, "CustomStringRep1")
//		macro(MY_ENUM_LIST2_V2, "CustomStringRep2")
//		macro(MY_ENUM_LIST2_V3, "CustomStringRep3")
//	RJD_ENUM_DECLARE_WITH_STRINGS(CoolEnum, MY_ENUM_LIST)
//	RJD_ENUM_DEFINE_WITH_STRINGS(CoolEnum, MY_ENUM_LIST)
//

#define RJD_ENUM_IMPL_TOSTRING(name) name ## _tostring
#define RJD_ENUM_IMPL_COUNT(name) k_ ## name ## _count
#define RJD_ENUM_IMPL_PARSE(name) name ## _parse
#define RJD_ENUM_IMPL_STRINGS(name) s_ ## name ## _strings

#define RJD_ENUM_IMPL_MEMBER(item) item,
#define RJD_ENUM_IMPL_SUM(item) 1 +
#define RJD_ENUM_IMPL_TOSTRING_ITEM(item) #item,
#define RJD_ENUM_IMPL_TOSTRING_CASE(item) case item: return #item;

#define RJD_ENUM_IMPL_MEMBER_WITH_STRING(item, str) item,
#define RJD_ENUM_IMPL_SUM_WITH_STRING(item, str) 1 +
#define RJD_ENUM_IMPL_WITH_STRING_ITEM(item, str) str,
#define RJD_ENUM_IMPL_WITH_STRING_CASE(item, str) case item: return str;

#define RJD_ENUM_DECLARE(name, macrolist)											\
	enum name {																		\
		macrolist(RJD_ENUM_IMPL_MEMBER)												\
	};																				\
	enum { RJD_ENUM_IMPL_COUNT(name) = macrolist(RJD_ENUM_IMPL_SUM) 0 };			\
	const char* RJD_ENUM_IMPL_TOSTRING(name)(enum name v);							\
	bool RJD_ENUM_IMPL_PARSE(name)(const char* s, enum name* out);					\
	extern const char* RJD_ENUM_IMPL_STRINGS(name)[];

#define RJD_ENUM_DEFINE(name, macrolist)											\
	const char* RJD_ENUM_IMPL_TOSTRING(name)(enum name v) {							\
		switch(v) {																	\
			macrolist(RJD_ENUM_IMPL_TOSTRING_CASE)									\
		}																			\
		return "invalid value";														\
	}																				\
	bool RJD_ENUM_IMPL_PARSE(name)(const char* s, enum name* out) {					\
		RJD_ASSERT(out);															\
		for (size_t i = 0; i < RJD_ENUM_IMPL_COUNT(name); ++i) {					\
			if (!strcmp(RJD_ENUM_IMPL_STRINGS(name)[i], s)) {						\
				*out = (enum name)i;												\
				return true;														\
			}																		\
		}																			\
		return false;																\
	}																				\
	const char* RJD_ENUM_IMPL_STRINGS(name)[] = {									\
		macrolist(RJD_ENUM_IMPL_TOSTRING_ITEM)										\
	};

#define RJD_ENUM_DECLARE_WITH_STRINGS(name, macrolist)								\
	enum name {																		\
		macrolist(RJD_ENUM_IMPL_MEMBER_WITH_STRING)									\
	};																				\
	enum { RJD_ENUM_IMPL_COUNT(name) = macrolist(RJD_ENUM_IMPL_SUM_WITH_STRING) 0 };\
	const char* RJD_ENUM_IMPL_TOSTRING(name)(enum name v);							\
	bool RJD_ENUM_IMPL_PARSE(name)(const char* s, enum name* out);					\
	extern const char* RJD_ENUM_IMPL_STRINGS(name)[];

#define RJD_ENUM_DEFINE_WITH_STRINGS(name, macrolist)								\
	const char* RJD_ENUM_IMPL_TOSTRING(name)(enum name v) {							\
		switch(v) {																	\
			macrolist(RJD_ENUM_IMPL_WITH_STRING_CASE)								\
		}																			\
		return "invalid value";														\
	}																				\
	bool RJD_ENUM_IMPL_PARSE(name)(const char* s, enum name* out) {					\
		RJD_ASSERT(out);															\
		for (size_t i = 0; i < RJD_ENUM_IMPL_COUNT(name); ++i) {					\
			if (!strcmp(RJD_ENUM_IMPL_STRINGS(name)[i], s)) {						\
				*out = (enum name)i;												\
				return true;														\
			}																		\
		}																			\
		return false;																\
	}																				\
	const char* RJD_ENUM_IMPL_STRINGS(name)[] = {									\
		macrolist(RJD_ENUM_IMPL_WITH_STRING_ITEM)									\
	};

#ifdef RJD_ENABLE_SHORTNAMES
	#define	ENUM_DECLARE	RJD_ENUM_DECLARE 
	#define ENUM_DEFINE		RJD_ENUM_DEFINE

	#define	ENUM_DECLARE_WITH_STRINGS	RJD_ENUM_DECLARE_WITH_STRINGS
	#define ENUM_DEFINE_WITH_STRINGS	RJD_ENUM_DEFINE_WITH_STRINGS
#endif

