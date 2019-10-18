# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

common: libcin_common.a
mixer: libcin_mixer.a

# No need to run ranlib on a single object
libcin_common.a: cin_soft_loader.o
	ar rc libcin_common.a cin_soft_loader.o

cin_soft_loader.o: cin_soft_loader.c cin_soft_loader.h $(ROOTDIR)/cinnamon.h $(ROOTDIR)/cin_export.h
	$(CC) $(CFLAGS) -I$(ROOTDIR) -c cin_soft_loader.c -o cin_soft_loader.o

MIXER_OBJECTS=cin_mixer_sound.o cin_dsp.o

libcin_mixer.a: $(MIXER_OBJECTS)
	rm mix_x.a 2> /dev/null || true
	$(AR) rc mix_x.a $(MIXER_OBJECTS)
	$(RANLIB) mix_x.a
	cp mix_x.a libcin_mixer.a

cin_mixer_sound.o: cin_mixer_sound.c cin_mixer_sound.h cin_dsp.h $(ROOTDIR)/cin_export.h $(ROOTDIR)/cin_format.h
	$(CC) $(CFLAGS) -I"$(ROOTDIR)" -c cin_mixer_sound.c -o cin_mixer_sound.o

cin_dsp.o: cin_dsp.c cin_dsp.h cin_convert_core.inc
	$(CC) $(CFLAGS) -c cin_dsp.c -o cin_dsp.o
