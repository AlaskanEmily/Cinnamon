# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# BSD makefile

.include "gcc.mk"

BACKEND?=oss

# Only OpenAL can be compiled without C++ currently.
.if "${BACKEND}" == "openal"
LINK?=$(CC)
.else
LINK=$(CXX)
.endif

.include "unix.mk"
