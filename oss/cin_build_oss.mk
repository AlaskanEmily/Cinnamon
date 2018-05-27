# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

OSS_OBJECTS=cin_driver_oss$(OBJ) cin_sound_oss$(OBJ) cin_dsp$(OBJ) cin_loader_soft$(OBJ) cin_thread_$(MIXER_TYPE)$(OBJ) cin_lock_$(MIXER_TYPE)$(OBJ) cin_driver_mixer$(OBJ) cin_driver_mixer_ops$(OBJ)
COMMON_OSS_DEPS=$(COMMON_DEPS)
CIN_LOADER_OSS_DEPS=$(COMMON_OSS_DEPS) oss$(PATHSEP)cin_driver_oss.cpp oss$(PATHSEP)cin_driver_oss.hpp
CIN_SOUND_OSS_DEPS=$(COMMON_OSS_DEPS) oss$(PATHSEP)cin_sound_oss.c oss$(PATHSEP)cin_sound_oss.h
