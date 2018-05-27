# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

DSOUND_OBJECTS=cin_driver_dsound$(OBJ) cin_loader_soft$(OBJ) cin_sound_dsound$(OBJ)
COMMON_DSOUND_DEPS=
CIN_DRIVER_DSOUND_DEPS=$(COMMON_DSOUND_DEPS) dsound$(PATHSEP)cin_driver_dsound.cpp cin_driver.h
CIN_SOUND_DSOUND_DEPS=$(COMMON_DSOUND_DEPS) dsound$(PATHSEP)cin_sound_dsound.cpp cin_sound.h cin_loader.h cin_loader_soft.h