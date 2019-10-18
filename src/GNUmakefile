# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# GNU makefile.

include gcc.mk

BACKEND?=openal

ifeq "$(BACKEND)" "dsound"
LINK?=$(CXX)
else ifeq "$(BACKEND)" "oss"
EXTRALIBS=-lpthread
LINK?="$(CC)"
else ifeq "$(BACKEND)" "openal"
EXTRALIBS=-lopenal
LINK?="$(CC)"
else
endif
LINK?="$(CC)"
include unix.mk
