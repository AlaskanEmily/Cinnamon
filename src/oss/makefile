# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

all: libcin_oss.a

OBJECTS=cin_oss.o cin_oss_driver.o cin_oss_sound.o

CIN_COMMONLIB=$(ROOTDIR)/common/libcin_common.a
CIN_MIXERLIB=$(ROOTDIR)/common/libcin_mixer.a

libcin_oss_x.a: $(OBJECTS)
	if [[ -f libcin_oss_x2.a ]] ; then rm libcin_oss_x2.a ; fi
	$(AR) rc libcin_oss_x2.a $(OBJECTS)
	$(RANLIB) libcin_oss_x2.a
	mv libcin_oss_x2.a libcin_oss_x.a

libcin_oss.a: $(CIN_COMMONLIB) $(CIN_MIXERLIB) libcin_oss_x.a
	if [[ -f libcin_oss2.a ]] ; then rm libcin_oss2.a ; fi
	echo $$'create libcin_oss2.a\naddlib libcin_oss_x.a\naddlib $(CIN_COMMONLIB)\naddlib $(CIN_MIXERLIB)\nsave\nend\n' | $(AR) M
	$(RANLIB) libcin_oss2.a
	mv libcin_oss2.a libcin_oss.a

COMMONHEADERS=../cinnamon.h ../cin_export.h ../cin_format.h

cin_oss.o: cin_oss.c cin_oss.h cin_oss_driver.h cin_oss_sound.h $(COMMONHEADERS) ../common/cin_soft_loader.h ../common/cin_dsp.h ../common/cin_mixer_sound.h
	$(CC) $(CFLAGS) -I"$(ROOTDIR)" -I"$(ROOTDIR)/common" -c cin_oss.c -o cin_oss.o

cin_oss_driver.o: cin_oss_driver.c cin_oss_driver.h cin_oss.h $(COMMONHEADERS)
	$(CC) $(CFLAGS) -I"$(ROOTDIR)" -c cin_oss_driver.c -o cin_oss_driver.o

cin_oss_sound.o: cin_oss_sound.c cin_oss.h $(COMMONHEADERS) ../common/cin_soft_loader.h ../common/cin_mixer_sound.h
	$(CC) $(CFLAGS) -I"$(ROOTDIR)" -I"$(ROOTDIR)/common" -c cin_oss_sound.c -o cin_oss_sound.o
