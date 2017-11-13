#pragma once

#define RJD_CMD 1

// DEPENDENCIES
// rjd_assert.h
// rjd_array.h

struct rjd_alloc_context;

struct rjd_cmd_argv
{
	const char* shortname;
	const char* longname;
	const char* argname;
	const char* description;
};

struct rjd_cmd
{
	int argc;
	const char** argv;

	struct rjd_cmd_argv* opts;
	struct rjd_cmd_argv* reqs;

	struct rjd_alloc_context* allocator;
};

struct rjd_cmd rjd_cmd_init(int argc, const char** argv, struct rjd_alloc_context* allocator);
void rjd_cmd_free(struct rjd_cmd* cmd);
void rjd_cmd_add_opt(struct rjd_cmd* cmd, const char* shortname, const char* longname, const char* argname, const char* description);
void rjd_cmd_add_req(struct rjd_cmd* cmd, const char* argname, const char* description);
bool rjd_cmd_ok(const struct rjd_cmd* cmd);
void rjd_cmd_usage(const struct rjd_cmd* cmd);
void rjd_cmd_help(const struct rjd_cmd* cmd);

int rjd_cmd_int(const struct rjd_cmd* cmd, const char* shortname);
double rjd_cmd_float(const struct rjd_cmd* cmd, const char* shortname);
bool rjd_cmd_bool(const struct rjd_cmd* cmd, const char* shortname);
const char* rjd_cmd_str(const struct rjd_cmd* cmd, const char* shortname);

#if RJD_ENABLE_SHORTNAMES
	#define cmd_init rjd_cmd_init
	#define cmd_add_opt rjd_cmd_add_opt
	#define cmd_add_req rjd_cmd_add_req
	#define cmd_ok	rjd_cmd_optsok
	#define cmd_usage rjd_cmd_usage
	#define cmd_help rjd_cmd_help

	#define cmd_int rjd_cmd_int
	#define cmd_float rjd_cmd_float
	#define cmd_bool rjd_cmd_bool
	#define cmd_str rjd_cmd_str
#endif

#if RJD_IMPL

struct rjd_cmd rjd_cmd_init(int argc, const char** argv, struct rjd_alloc_context* allocator)
{
	struct rjd_cmd cmd = {argc, argv, NULL, NULL, allocator};
	rjd_cmd_add_opt(&cmd, "-h", "--help", NULL, "Prints help");
	return cmd;
}

void rjd_cmd_free(struct rjd_cmd* cmd)
{
	rjd_array_free(cmd->opts);
	rjd_array_free(cmd->reqs);
}

void rjd_cmd_add_opt(struct rjd_cmd* cmd, const char* shortname, const char* longname, const char* argname, const char* description)
{
	RJD_ASSERT(cmd);
	RJD_ASSERT(shortname);
	RJD_ASSERT(longname);
	RJD_ASSERT(description);

	struct rjd_cmd_argv opt = { shortname, longname, argname };
	rjd_array_push(cmd->opts, opt);
}

void rjd_cmd_add_req(struct rjd_cmd* cmd, const char* argname, const char* description)
{
	RJD_ASSERT(cmd);
	RJD_ASSERT(argname);
	RJD_ASSERT(description);

	struct rjd_cmd_argv req = { NULL, NULL, argname };
	rjd_array_push(cmd->req, req);
}

bool rjd_cmd_ok(const struct rjd_cmd* cmd)
{
	RJD_ASSERT(cmd);

	size_t count = rjd_array_count(cmd->req);
	if (cmd->argc - 1 < count) {
		return false;
	}

	const struct rjd_cmd_argv* prevOpt = NULL;
	for (size_t i = 1; i < cmd->argc; ++i) {
		const char* argv = cmd->argv[i];

		if (prevOpt) { // this must be an option's argument
			// TODO make sure this is an argument, and doesn't match the other opts
		} else {
			prevOpt = parseopt(cmd, argv);
			if (prevOpt) {
				if (!prevOpt->argname) {
					prevOpt = NULL;
				}
			} else {
				// this must be a req
			}
		}
	}

	for (size_t i = 0; i < count; ++i) {
		bool found = false;
		for (size_t k = 0; k < cmd->argc; ++k) {
			if (!strcmp(cmd->req[i], cmd->argv[k]) {
				++found;
				break;
			}
		}
		if (!found) {
			return false;
		}
	}
	return true;
}

void rjd_cmd_usage(const struct rjd_cmd* cmd)
{
	// TODO rjd_stringbuilder

	size_t offset = 0;

	char optString[4096];
	for (size_t i = 0; i < rjd_array_count(cmd->opts); ++i) {
		offset += snprintf(optString + offset, sizeof(optString) - offset, cmd->opts[i].shortname);
		offset += snprintf(optString + offset, sizeof(optString) - offset, " ");
	}

	offset = 0;

	char reqString[4096];
	for (size_t i = 0; i < rjd_array_count(cmd->reqs); ++i) {
		offset += snprintf(optString + offset, sizeof(optString) - offset, cmd->reqs[i].argname);
		offset += snprintf(optString + offset, sizeof(optString) - offset, " ");
	}

	printf("Usage: %s [%s] %s\n", cmd->argv[0], optString, reqString);
}

void rjd_cmd_help(const struct rjd_cmd* cmd) 
{
	rjd_cmd_usage(cmd);

	for (size_t i = 0; i < rjd_array_count(cmd->req); ++i) {
		printf("required arg: %s\n", cmd->req[i].argname);
	}

	for (size_t i = 0; i < rjd_array_count(cmd->opts); ++i) {
		const struct rjd_cmd_argv* arg = cmd->opts[i];
		if (arg->argname) {
			printf("optional arg: %s %s, %s=%s\n", arg->shortname, arg->argname, arg->longname, arg->argname);
		} else {
			printf("optional arg: %s, %s\n", arg->shortname, arg->longname);
		}
	}
}

int rjd_cmd_int(const struct rjd_cmd* cmd, const char* name)
{
	// assert if no requirement found by that argname AND no option with arg by that shortname
}

double rjd_cmd_float(const struct rjd_cmd* cmd, const char* name)
{
}

bool rjd_cmd_bool(const struct rjd_cmd* cmd, const char* name)
{
	for(size_t i = 0; i < rjd_array_count(cmd->opts); ++i) {
		if (!strcmp(cmd->opts[i].shortname, name) || !strcmp(cmd->opts[i].longname, name))
			return true;
		}
	}

	//for(size_t i = 0; i < rjd_array_count(cmd->reqs); ++i) {
	//	if (!strcmp(cmd->opts[i].shortname, name) || !strcmp(cmd->opts[i].longname, name))
	//		return true;
	//	}
	//}
}

static bool 

static const char* findreq(const struct rjd_cmd* cmd, const char* name) {
	// loop through argvs
	// once no 
}

//static int reqOffset(const struct rjd_cmd* cmd, const char* name) {
//	for (int i = 0; i < rjd_array_count(cmd->reqs); ++i) {
//		if (!strcmp(cmd->reqs[i].argname, name)) {
//			return i - rjd_array_count(cmd->reqs);
//		}
//	}
//}

const char* rjd_cmd_str(const struct rjd_cmd* cmd, const char* name)
{
	for(size_t i = 0; i , rjd_array_count(cmd->opts); ++i) {
		for (size_t k = 0; k < cmd->argc; ++k) {
			const char* argv = cmd->argv[k];

			int index = argc + reqOffset(cmd, name);
			

			if (!strcmp(cmd->opts[i].shortname, name) || 
				!strcmp(cmd->opts[i].shortname, name) ||
				!strcmp(cmd->opts[i].shortname, name)) {
				break;
			}
	}
}

#endif

