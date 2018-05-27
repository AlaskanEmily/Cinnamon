# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

CC=gcc
CXX=g++
CCFLAGS=-Wall -Wextra -pedantic -Wshadow -Wenum-compare -Wno-switch -g -fPIC -Iinclude -I./ -I./mixer
CFLAGS=$(CCFLAGS) -ansi
CXXFLAGS=$(CCFLAGS) -std=c++98 -fno-rtti -fno-exceptions
LINKFLAGS=-g -fPIC

MIXER_TYPE=unix

# Common objects
cin_common.o: $(CIN_COMMON_DEPS)
	$(CC) $(CFLAGS) -c cin_common.c -o cin_common.o

cin_loader_soft.o: $(CIN_LOADER_SOFT_DEPS)
	$(CC) $(CFLAGS) -c cin_loader_soft.c -o cin_loader_soft.o

# DSP object
cin_dsp.o: dsp/cin_dsp.cpp dsp/cin_dsp.h
	$(CXX) $(CXXFLAGS) -c dsp/cin_dsp.cpp -o cin_dsp.o

# Mixer objects 
cin_lock_unix.o: mixer/unix/cin_lock_unix.c mixer/cin_lock.h
	$(CC) $(CFLAGS) -c mixer/unix/cin_lock_unix.c -o cin_lock_unix.o

cin_thread_unix.o: mixer/unix/cin_thread_unix.c mixer/cin_thread.h
	$(CC) $(CFLAGS) -c mixer/unix/cin_thread_unix.c -o cin_thread_unix.o

cin_driver_mixer.o: mixer/cin_driver_mixer.cpp mixer/cin_driver_mixer.hpp mixer/cin_thread.h mixer/cin_lock.h
	$(CXX) $(CXXFLAGS) -c mixer/cin_driver_mixer.cpp -o cin_driver_mixer.o

cin_driver_mixer_ops.o: mixer/cin_driver_mixer_ops.cpp mixer/cin_driver_mixer_ops.h mixer/cin_driver_mixer.hpp 
	$(CXX) $(CXXFLAGS) -c mixer/cin_driver_mixer_ops.cpp -o cin_driver_mixer_ops.o

# OpenAL objects
cin_loader_openal.o: $(CIN_LOADER_OPENAL_DEPS)
	$(CC) $(CFLAGS) -c openal/cin_loader_openal.c -o cin_loader_openal.o

cin_driver_openal.o: $(CIN_DRIVER_OPENAL_DEPS)
	$(CC) $(CFLAGS) -c openal/cin_driver_openal.c -o cin_driver_openal.o

cin_sound_openal.o: $(CIN_SOUND_OPENAL_DEPS)
	$(CC) $(CFLAGS) -c openal/cin_sound_openal.c -o cin_sound_openal.o

# OSS objects
cin_sound_oss.o: $(CIN_SOUND_OSS_DEPS)
	$(CC) $(CFLAGS) -c oss/cin_sound_oss.c -o cin_sound_oss.o

cin_driver_oss.o: $(CIN_DRIVER_OSS_DEPS)
	$(CXX) $(CXXFLAGS) -c oss/cin_driver_oss.cpp -o cin_driver_oss.o

#Test objects
sine_test.o: $(SINE_TEST_DEPS)
	$(CC) $(CFLAGS) -c sine_test.c -o sine_test.o

openal: $(OPENAL_OBJECTS) cin_common.o $(TEST_OBJS)
	ar rc x.a $(OPENAL_OBJECTS)
	ranlib x.a
	mv x.a libcinnamon.a
	$(CC) sine_test.o -L./ libcinnamon.a -lopenal

oss: $(OSS_OBJECTS) cin_common.o $(TEST_OBJS)
	ar rc x.a $(OSS_OBJECTS)
	ranlib x.a
	mv x.a libcinnamon.a
	$(CXX) sine_test.o -L./ libcinnamon.a

dsound: cinnamon.dll
