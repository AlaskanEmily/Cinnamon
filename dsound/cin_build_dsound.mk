# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

DSOUND_OBJECTS=cin_driver_dsound$(OBJ) cin_loader_soft$(OBJ) cin_sound_dsound$(OBJ) cin_lock_$(MIXER_TYPE)$(OBJ) cin_thread_$(MIXER_TYPE)$(OBJ) cin_driver_mixer$(OBJ)
CIN_DRIVER_DSOUND_DEPS=$(COMMON_DSOUND_DEPS) dsound$(PATHSEP)cin_driver_dsound.cpp dsound$(PATHSEP)cin_driver_dsound.hpp cin_driver.h mixer$(PATHSEP)cin_lock.h mixer$(PATHSEP)cin_thread.h
CIN_SOUND_DSOUND_DEPS=$(COMMON_DSOUND_DEPS) dsound$(PATHSEP)cin_sound_dsound.cpp dsound$(PATHSEP)cin_sound_dsound.hpp cin_sound.h cin_loader.h cin_loader_soft.h
