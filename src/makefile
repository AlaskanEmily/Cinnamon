# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# BSD makefile

.include "gcc.mk"

BACKEND?=oss

.if "${BACKEND}" == "dsound"
LINK?=$(CXX)
.else
LINK=$(CC)
.endif

.include "unix.mk"
