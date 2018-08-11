includes = [
    "<stdint.h>",
    "<float.h>",
    "<stddef.h>",
    "<stdbool.h>",
    "<math.h>",
    "<string.h>",
    "<stdarg.h>",
    "<xmmintrin.h> // SSE2",
    "<pmmintrin.h> // SSE3"
]

includes_impl = [
    "<stdlib.h>",
    "<stdio.h>",
]

files = [ 
    "rjd_platform.h",
    "rjd_debug.h",
    "rjd_result.h",
    "rjd_enum.h",
    "rjd_hash.h",
    "rjd_mem.h",
    "rjd_rng.h",
    "rjd_array.h",
    "rjd_math.h",
    "rjd_geo.h",
    "rjd_easing.h",
    "rjd_strbuf.h",
    "rjd_profiler.h",
    "rjd_cmd.h",
    "rjd_dict.h",
    "rjd_fio.h",
    "rjd_strpool.h",
    "rjd_slotmap.h"
]

concat = "#pragma once\n\n"

concat += "////////////////////////////////////////////////////////////////////////////////\n"
concat += "// autogenerated rjd.h\n"
concat += "////////////////////////////////////////////////////////////////////////////////\n\n"
for include in includes:
    concat += "#include " + include + "\n"

concat += "\n#if RJD_IMPL\n"

for include in includes_impl:
    concat += "\t#include " + include + "\n"

concat += "#endif\n\n"

for filename in files:
    with open(filename, "r") as file:
        data = file.read()
        concat += "////////////////////////////////////////////////////////////////////////////////\n"
        concat += "// " + filename + "\n"
        concat += "////////////////////////////////////////////////////////////////////////////////\n"
        concat += "\n"
        concat += data;
        concat += "\n"

with open("rjd.h", "w") as file:
    file.write(concat)

with open("rjd_all.h", "w") as file:
    allh = "#pragma once\n\n"
    for include in includes:
        allh += "#include " + include + "\n"
    for include in includes_impl:
        allh += "#include " + include + "\n"
    allh += "\n"
    for include in files:
        allh += "#include \"" + include + "\"\n"
    file.write(allh)
