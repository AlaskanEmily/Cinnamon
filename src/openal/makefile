# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

all: libcin_openal.a

# No need to run ranlib on a single object
libcin_openal.a: cin_openal.o
	$(AR) rc libcin_openal.a cin_openal.o

cin_openal.o: cin_openal.c cin_openal.h
	$(CC) $(CFLAGS) -I"$(ROOTDIR)" -c cin_openal.c -o cin_openal.o
