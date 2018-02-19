# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

OPENAL_OBJECTS=cin_driver_openal$(OBJ) cin_loader_openal$(OBJ) cin_sound_openal$(OBJ)
COMMON_OPENAL_DEPS=cin_openal.h cin_loader.h $(COMMON_DEPS)
CIN_LOADER_OPENAL_DEPS=$(COMMON_OPENAL_DEPS) cin_loader_openal.c
CIN_DRIVER_OPENAL_DEPS=$(COMMON_OPENAL_DEPS) cin_driver_openal.c cin_driver.h
CIN_SOUND_OPENAL_DEPS=$(COMMON_OPENAL_DEPS) cin_sound_openal.c cin_sound.h
