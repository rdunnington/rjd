#define RJD_BINRW_H 1

struct rjd_istream;
struct rjd_ostream;

struct rjd_binrw_state
{
	struct rjd_istream* istream;
	struct rjd_ostream* ostream;
};

static inline struct rjd_result rjd_binrw_read_int8(struct rjd_istream* stream, int8_t* value);
static inline struct rjd_result rjd_binrw_read_int16(struct rjd_istream* stream, int16_t* value);
static inline struct rjd_result rjd_binrw_read_int32(struct rjd_istream* stream, int32_t* value);
static inline struct rjd_result rjd_binrw_read_int64(struct rjd_istream* stream, int64_t* value);
static inline struct rjd_result rjd_binrw_read_uint8(struct rjd_istream* stream, uint8_t* value);
static inline struct rjd_result rjd_binrw_read_uint16(struct rjd_istream* stream, uint16_t* value);
static inline struct rjd_result rjd_binrw_read_uint32(struct rjd_istream* stream, uint32_t* value);
static inline struct rjd_result rjd_binrw_read_uint64(struct rjd_istream* stream, uint64_t* value);

static inline struct rjd_result rjd_binrw_write_int8(struct rjd_ostream* stream, int8_t value);
static inline struct rjd_result rjd_binrw_write_int16(struct rjd_ostream* stream, int16_t value);
static inline struct rjd_result rjd_binrw_write_int32(struct rjd_ostream* stream, int32_t value);
static inline struct rjd_result rjd_binrw_write_int64(struct rjd_ostream* stream, int64_t value);
static inline struct rjd_result rjd_binrw_write_uint8(struct rjd_ostream* stream, uint8_t value);
static inline struct rjd_result rjd_binrw_write_uint16(struct rjd_ostream* stream, uint16_t value);
static inline struct rjd_result rjd_binrw_write_uint32(struct rjd_ostream* stream, uint32_t value);
static inline struct rjd_result rjd_binrw_write_uint64(struct rjd_ostream* stream, uint64_t value);

static inline struct rjd_result rjd_binrw_readwrite_int8(struct rjd_binrw_state* state, int8_t* value);
static inline struct rjd_result rjd_binrw_readwrite_int16(struct rjd_binrw_state* state, int16_t* value);
static inline struct rjd_result rjd_binrw_readwrite_int32(struct rjd_binrw_state* state, int32_t* value);
static inline struct rjd_result rjd_binrw_readwrite_int64(struct rjd_binrw_state* state, int64_t* value);
static inline struct rjd_result rjd_binrw_readwrite_uint8(struct rjd_binrw_state* state, uint8_t* value);
static inline struct rjd_result rjd_binrw_readwrite_uint16(struct rjd_binrw_state* state, uint16_t* value);
static inline struct rjd_result rjd_binrw_readwrite_uint32(struct rjd_binrw_state* state, uint32_t* value);
static inline struct rjd_result rjd_binrw_readwrite_uint64(struct rjd_binrw_state* state, uint64_t* value);

////////////////////////////////////////////////////////////////////////////////
// implementation

#define RJD_BINRW_DEFINE_READ_IMPL(type, name)										\
	static inline struct rjd_result name(struct rjd_istream* stream, type* value) {	\
		RJD_ASSERT(stream && value);												\
		return rjd_istream_read(stream, value, sizeof(*value));						\
	}

#define RJD_BINRW_DEFINE_WRITE_IMPL(type, name)										\
	static inline struct rjd_result name(struct rjd_ostream* stream, type value) {	\
		RJD_ASSERT(stream);															\
		return rjd_ostream_write(stream, &value, sizeof(value));					\
	}

#define RJD_BINRW_DEFINE_READWRITE_IMPL(type, type2, name)								\
	static inline struct rjd_result name(struct rjd_binrw_state* state, type* value) {	\
		RJD_ASSERT(state && value && (state->istream || state->ostream));				\
		if (state->istream) {															\
			return rjd_binrw_read_ ## type2(state->istream, value);						\
		} else {																		\
			return rjd_binrw_write_ ## type2(state->ostream, *value);					\
		}																				\
	}

RJD_BINRW_DEFINE_READ_IMPL(int8_t, rjd_binrw_read_int8)
RJD_BINRW_DEFINE_READ_IMPL(int16_t, rjd_binrw_read_int16)
RJD_BINRW_DEFINE_READ_IMPL(int32_t, rjd_binrw_read_int32)
RJD_BINRW_DEFINE_READ_IMPL(int64_t, rjd_binrw_read_int64)
RJD_BINRW_DEFINE_READ_IMPL(uint8_t, rjd_binrw_read_uint8)
RJD_BINRW_DEFINE_READ_IMPL(uint16_t, rjd_binrw_read_uint16)
RJD_BINRW_DEFINE_READ_IMPL(uint32_t, rjd_binrw_read_uint32)
RJD_BINRW_DEFINE_READ_IMPL(uint64_t, rjd_binrw_read_uint64)

RJD_BINRW_DEFINE_WRITE_IMPL(int8_t, rjd_binrw_write_int8)
RJD_BINRW_DEFINE_WRITE_IMPL(int16_t, rjd_binrw_write_int16)
RJD_BINRW_DEFINE_WRITE_IMPL(int32_t, rjd_binrw_write_int32)
RJD_BINRW_DEFINE_WRITE_IMPL(int64_t, rjd_binrw_write_int64)
RJD_BINRW_DEFINE_WRITE_IMPL(uint8_t, rjd_binrw_write_uint8)
RJD_BINRW_DEFINE_WRITE_IMPL(uint16_t, rjd_binrw_write_uint16)
RJD_BINRW_DEFINE_WRITE_IMPL(uint32_t, rjd_binrw_write_uint32)
RJD_BINRW_DEFINE_WRITE_IMPL(uint64_t, rjd_binrw_write_uint64)

RJD_BINRW_DEFINE_READWRITE_IMPL(int8_t, int8, rjd_binrw_readwrite_int8)
RJD_BINRW_DEFINE_READWRITE_IMPL(int16_t, int16, rjd_binrw_readwrite_int16)
RJD_BINRW_DEFINE_READWRITE_IMPL(int32_t, int32, rjd_binrw_readwrite_int32)
RJD_BINRW_DEFINE_READWRITE_IMPL(int64_t, int64, rjd_binrw_readwrite_int64)
RJD_BINRW_DEFINE_READWRITE_IMPL(uint8_t, uint8, rjd_binrw_readwrite_uint8)
RJD_BINRW_DEFINE_READWRITE_IMPL(uint16_t, uint16, rjd_binrw_readwrite_uint16)
RJD_BINRW_DEFINE_READWRITE_IMPL(uint32_t, uint32, rjd_binrw_readwrite_uint32)
RJD_BINRW_DEFINE_READWRITE_IMPL(uint64_t, uint64, rjd_binrw_readwrite_uint64)

