# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

COMMON_DEPS=cin_driver.h cin_format.h cin_export.h cin_common.h
TEST_OBJS=sine_test$(OBJ)
SINE_TEST_DEPS=$(COMMON_DEPS) cin_loader.h cin_sound.h sine_test.c
CIN_COMMON_DEPS=cin_common.c cin_common.h $(COMMON_DEPS) cin_loader.h cin_sound.h
CIN_LOADER_SOFT_DEPS=cin_loader_soft.c cin_loader_soft.h cin_loader.h
