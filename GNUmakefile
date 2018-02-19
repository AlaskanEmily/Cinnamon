# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

OBJ?=.o
SO?=.so
A?=.a
LIB?=lib
CP?=cp
MV?=mv
MVFLAGS?=
CPFLAGS?=

CC?=gcc
CXX?=g++
AR?=ar
RANLIB?=ranlib
LINKER?=$(CXX)
SHAREDFLAG?=-shared
LINKFLAGS?=-g
CCFLAGS=-Wall -Wextra -pedantic -Wshadow -Werror -g -DCIN_INTERNAL

CFLAGS?=
CFLAGS+=-ansi -Wenum-compare -Os

CXXFLAGS?=
CXXFLAGS+=-fno-rtti -fno-exceptions -O2

include cin_build.mk
include cin_build_openal.mk

openal: $(OPENAL_OBJECTS) cin_common.o $(TEST_OBJS)
	$(LINK) $(OPENAL_OBJECTS) cin_common.o -lopenal $(LINKFLAGS) -o $(LIB)cinnamon$(SO)
	$(CP) $(CPFLAGS) $(LIB)cinnamon$(SO) $(LIB)cinnamon_openal$(SO)
	$(AR) rc x.a $(OPENAL_OBJECTS) cin_common.o
	$(RANLIB) x.a
	$(CP) $(CPFLAGS) x.a $(LIB)cinnamon_openal$(A)
	$(MV) $(MVFLAGS) x.a $(LIB)cinnamon$(A)

# Common objects
cin_common.o: $(CIN_COMMON_DEPS)
	$(CC) $(CFLAGS) cin_common.c -o cin_common.o

cin_loader_soft.o: $(CIN_LOADER_SOFT_DEPS)
	$(CC) $(CFLAGS) -c cin_loader_soft.c -o cin_loader_soft.o

# OpenAL objects
cin_loader_openal.o: $(CIN_LOADER_OPENAL_DEPS)
	$(CC) $(CFLAGS) -c cin_loader_openal.c -o cin_loader_openal.o

cin_driver_openal.o: $(CIN_DRIVER_OPENAL_DEPS)
	$(CC) $(CFLAGS) -c cin_driver_openal.c -o cin_driver_openal.o

cin_sound_openal.o: $(CIN_SOUND_OPENAL_DEPS)
	$(CC) $(CFLAGS) -c cin_sound_openal.c -o cin_sound_openal.o

#Test objects
sine_test.o: $(SINE_TEST_DEPS)
	$(CC) $(CFLAGS) -c sine_test.c

.PHONY: openal
