# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

all: libcinnamon.a sine_test aucat

CIN_BACKEND?=$(BACKEND)

common_lib:
	$(MAKE) -C common common CC=$(CC) CFLAGS="$(CFLAGS)" AR=$(AR) RANLIB=$(RANLIB) ROOTDIR="$(CURDIR)"

mixer_lib:
	$(MAKE) -C common mixer CC=$(CC) CFLAGS="$(CFLAGS)" AR=$(AR) RANLIB=$(RANLIB) ROOTDIR="$(CURDIR)"

openal_lib:
	$(MAKE) -C openal CC=$(CC) CFLAGS="$(CFLAGS)" AR=$(AR) RANLIB=$(RANLIB) ROOTDIR="$(CURDIR)"

oss_lib: mixer_lib common_lib
	$(MAKE) -C oss CC=$(CC) CFLAGS="$(CFLAGS)" AR=$(AR) RANLIB=$(RANLIB) ROOTDIR="$(CURDIR)"

libcinnamon.a: $(CIN_BACKEND)_lib
	cp $(CIN_BACKEND)/libcin_$(CIN_BACKEND).a libcinnamon.a

sine_test.o: sine_test.c cinnamon.h cin_export.h cin_format.h
	$(CC) $(CFLAGS) -c sine_test.c -o sine_test.o
    
aucat.o: aucat.c cinnamon.h cin_export.h cin_format.h
	$(CC) $(CFLAGS) -c aucat.c -o aucat.o

sine_test: libcinnamon.a sine_test.o
	$(LINK) sine_test.o libcinnamon.a -o sine_test

aucat: libcinnamon.a aucat.o
	$(LINK) aucat.o libcinnamon.a -o aucat

clean:
	rm *.o 2> /dev/null || true
	rm *.a 2> /dev/null || true
	rm *.exe 2> /dev/null || true # For cygwin
	rm */*.a 2> /dev/null || true
	rm */*.o 2> /dev/null || true
	rm */*.so 2> /dev/null || true

.PHONY: openal_lib oss_lib mixer_lib common_lib clean
.SILENT: clean
.IGNORE: clean
