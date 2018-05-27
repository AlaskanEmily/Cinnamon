# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

CC=gcc
CXX=g++
CCFLAGS=-Wall -Wextra -pedantic -Wshadow -Wenum-compare -Wno-switch -g -fPIC -Iinclude -I./
CFLAGS=$(CCFLAGS) -ansi
CXXFLAGS=$(CCFLAGS) -std=c++98 -fno-rtti -fno-exceptions
LINKFLAGS=-g -fPIC

# Common objects
cin_common.o: $(CIN_COMMON_DEPS)
	$(CC) $(CFLAGS) cin_common.c -o cin_common.o

# OpenAL objects
cin_loader_openal.o: $(CIN_LOADER_OPENAL_DEPS)
	$(CC) $(CFLAGS) -c openal/cin_loader_openal.c -o cin_loader_openal.o

cin_driver_openal.o: $(CIN_DRIVER_OPENAL_DEPS)
	$(CC) $(CFLAGS) -c openal/cin_driver_openal.c -o cin_driver_openal.o

cin_sound_openal.o: $(CIN_SOUND_OPENAL_DEPS)
	$(CC) $(CFLAGS) -c openal/cin_sound_openal.c -o cin_sound_openal.o

#Test objects
sine_test.o: $(SINE_TEST_DEPS)
	$(CC) $(CFLAGS) -c sine_test.c -o sine_test.o

openal: $(OPENAL_OBJECTS) cin_common.o $(TEST_OBJS)
	ar rc x.a $(OPENAL_OBJECTS)
	ranlib x.a
	mv x.a libcinnamon.a
	$(CC) sine_test.o -L./ libcinnamon.a -lopenal

dsound: cinnamon.dll
