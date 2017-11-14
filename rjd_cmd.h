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

int rjd_cmd_int(const struct rjd_cmd* cmd, const char* shortname, int _default);
unsigned rjd_cmd_uint(const struct rjd_cmd* cmd, const char* shortname, unsigned _default);
double rjd_cmd_float(const struct rjd_cmd* cmd, const char* shortname, double _default);
bool rjd_cmd_bool(const struct rjd_cmd* cmd, const char* shortname);
const char* rjd_cmd_str(const struct rjd_cmd* cmd, const char* shortname);

#if RJD_ENABLE_SHORTNAMES
	#define cmd_init	rjd_cmd_init
	#define cmd_free	rjd_cmd_free
	#define cmd_add_opt	rjd_cmd_add_opt
	#define cmd_add_req	rjd_cmd_add_req
	#define cmd_ok		rjd_cmd_ok
	#define cmd_usage	rjd_cmd_usage
	#define cmd_help	rjd_cmd_help

	#define cmd_int		rjd_cmd_int
	#define cmd_uint	rjd_cmd_uint
	#define cmd_float	rjd_cmd_float
	#define cmd_bool	rjd_cmd_bool
	#define cmd_str		rjd_cmd_str
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

	struct rjd_cmd_argv opt = { shortname, longname, argname, description };
	rjd_array_push(cmd->opts, opt);
}

void rjd_cmd_add_req(struct rjd_cmd* cmd, const char* argname, const char* description)
{
	RJD_ASSERT(cmd);
	RJD_ASSERT(argname);
	RJD_ASSERT(description);

	struct rjd_cmd_argv req = { NULL, NULL, argname, description };
	rjd_array_push(cmd->reqs, req);
}

static const struct rjd_cmd_argv* rjd_cmd_matchopt(const struct rjd_cmd* cmd, const char* argv);
static int rjd_cmd_lastopt(const struct rjd_cmd* cmd);
static const struct rjd_cmd_argv* rjd_cmd_getopt(const struct rjd_cmd* cmd, const char* shortname);
static const char* rjd_cmd_findopt(const struct rjd_cmd* cmd, const char* shortname);
static const char* rjd_cmd_findreq(const struct rjd_cmd* cmd, const char* argname);

bool rjd_cmd_ok(const struct rjd_cmd* cmd)
{
	RJD_ASSERT(cmd);

	int count = rjd_array_count(cmd->reqs);
	if (cmd->argc - 1 < count) {
		return false;
	}

	const int lastopt = rjd_cmd_lastopt(cmd);

	for (int i = 1; i < lastopt; ++i) {
		const struct rjd_cmd_argv* opt = rjd_cmd_matchopt(cmd, cmd->argv[i]);

		if (!opt) {
			return false;
		}

		if (opt->argname) {
			if (!strcmp(cmd->argv[i], opt->shortname)) {
				// since we're expecting an argument, this shouldn't match any other options
				if (rjd_cmd_matchopt(cmd, cmd->argv[i+1])) {
					return false;
				}
			} else {
				const char* eq = strstr(cmd->argv[i], "=");
				if (!eq) {
					return false;
				}
				const char* arg = eq + 1;
				if (*arg == 0) {
					return false;
				}
			}
			++i;
		}
	}
	
	return (cmd->argc - lastopt) == (int) rjd_array_count(cmd->reqs);
}

void rjd_cmd_usage(const struct rjd_cmd* cmd)
{
	// TODO rjd_stringbuilder

	size_t offset = 0;

	char optString[4096];
	for (size_t i = 0; i < rjd_array_count(cmd->opts); ++i) {
		offset += snprintf(optString + offset, sizeof(optString) - offset, "%s", cmd->opts[i].shortname);
		offset += snprintf(optString + offset, sizeof(optString) - offset, " ");
	}

	offset = 0;

	char reqString[4096];
	for (size_t i = 0; i < rjd_array_count(cmd->reqs); ++i) {
		offset += snprintf(optString + offset, sizeof(optString) - offset, "%s", cmd->reqs[i].argname);
		offset += snprintf(optString + offset, sizeof(optString) - offset, " ");
	}

	printf("Usage: %s [%s] %s\n", cmd->argv[0], optString, reqString);
}

void rjd_cmd_help(const struct rjd_cmd* cmd) 
{
	rjd_cmd_usage(cmd);

	for (size_t i = 0; i < rjd_array_count(cmd->reqs); ++i) {
		printf("required arg: %s\n", cmd->reqs[i].argname);
	}

	for (size_t i = 0; i < rjd_array_count(cmd->opts); ++i) {
		const struct rjd_cmd_argv* arg = cmd->opts + i;
		if (arg->argname) {
			printf("optional arg: %s %s, %s=%s\n", arg->shortname, arg->argname, arg->longname, arg->argname);
		} else {
			printf("optional arg: %s, %s\n", arg->shortname, arg->longname);
		}
	}
}

int rjd_cmd_int(const struct rjd_cmd* cmd, const char* name, int _default)
{
	return (int)rjd_cmd_float(cmd, name, _default);
}

unsigned rjd_cmd_uint(const struct rjd_cmd* cmd, const char* name, unsigned _default)
{
	double v = rjd_cmd_float(cmd, name, _default);
	if (v < 0) {
		return _default;
	}
	return (unsigned)v;
}

double rjd_cmd_float(const struct rjd_cmd* cmd, const char* name, double _default)
{
	const char* str = rjd_cmd_str(cmd, name);
	if (!str) {
		return _default;
	}

	char* end = NULL;
	double v = strtod(str, &end);
	if (v == 0 && end != NULL) {
		return _default;
	}
	return v;
}

bool rjd_cmd_bool(const struct rjd_cmd* cmd, const char* name)
{
	const char* str = rjd_cmd_str(cmd, name);
	if (!str) {
		return false;
	}

	if (!strcmp(str, "true")) {
		return true;
	} else if (!strcmp(str, "false")) {
		return false;
	}

	const struct rjd_cmd_argv* opt = rjd_cmd_getopt(cmd, name);
	return opt && !strcmp(opt->shortname, name);
}

const char* rjd_cmd_str(const struct rjd_cmd* cmd, const char* name)
{
	const char* opt = rjd_cmd_findopt(cmd, name);
	if (opt) {
		return opt;
	}

	const char* req = rjd_cmd_findreq(cmd, name);
	if (req) {
		return req;
	}

	return NULL;
}

static const struct rjd_cmd_argv* rjd_cmd_matchopt(const struct rjd_cmd* cmd, const char* argv)
{
	for (uint32_t i = 0; i < rjd_array_count(cmd->opts); ++i) {
		const char* shortname = cmd->opts[i].shortname;
		const char* longname = cmd->opts[i].longname;
		if (!strcmp(shortname, argv)) {
			return cmd->opts + i;
		}
		if (strstr(argv, longname) == argv) {
			return cmd->opts + i;
		}
	}

	return NULL;
}

static int rjd_cmd_lastopt(const struct rjd_cmd* cmd)
{
	int last = 1;
	for (int i = 1; i < cmd->argc; ++i) {
		const struct rjd_cmd_argv* opt = rjd_cmd_matchopt(cmd, cmd->argv[i]);
		if (opt) {
			// skip the argument (assuming the format is ok)
			if (strcmp(cmd->argv[i], opt->shortname) && opt->argname) {
				++i;
			}
			last = i;
		} else {
			break;
		}
	}

	return last;
}

static const struct rjd_cmd_argv* rjd_cmd_getopt(const struct rjd_cmd* cmd, const char* shortname)
{
	for (uint32_t i = 0; i < rjd_array_count(cmd->opts); ++i) {
		if (!strcmp(cmd->opts[i].shortname, shortname)) {
			return cmd->opts + i;
		}
	}
	return NULL;
}

static const char* rjd_cmd_findopt(const struct rjd_cmd* cmd, const char* shortname)
{
	for (int i = 0; i < cmd->argc; ++i) {
		const struct rjd_cmd_argv* opt = rjd_cmd_matchopt(cmd, cmd->argv[i]);
		if (opt && !strcmp(opt->shortname, shortname)) {
			if (!opt->argname) {
				return cmd->argv[i];
			}
			if (!strcmp(cmd->argv[i], opt->shortname)) {
				return cmd->argv[i + 1];
			}
			const char* eq = strstr(cmd->argv[i], "=");
			if (eq) {
				return eq + 1;
			}
			break;
		}
	}
	return NULL;
}

static const char* rjd_cmd_findreq(const struct rjd_cmd* cmd, const char* argname) 
{
	int reqindex = -1;
	for (int i = 0; i < (int)rjd_array_count(cmd->reqs); ++i) {
		if (!strcmp(cmd->reqs[i].argname, argname)) {
			reqindex = i;
			break;
		}
	}

	if (reqindex == -1) {
		return NULL;
	}

	int optindex = rjd_cmd_lastopt(cmd);

	int argvindex = optindex + reqindex;
	RJD_ASSERT(argvindex < cmd->argc);

	return cmd->argv[argvindex];
}

#endif

