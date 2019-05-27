# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# GCC optoins for compilation.

CIN_FPICFLAGS?= -fPIC
CIN_DEBUGFLAGS?= -g
CC?=gcc
CXX?=g++
CCFLAGS?=-Wall -Wextra -pedantic -Wshadow -Wenum-compare -Wno-switch $(CIN_DEBUGFLAGS) $(CIN_FPICFLAGS)
CFLAGS?=$(CCFLAGS) -ansi
CXXFLAGS?=$(CCFLAGS) -std=c++98 -fno-rtti -fno-exceptions
AR?=ar
RANLIB?=ranlib
LINKFLAGS?=$(CIN_DEBUGFLAGS) $(CIN_FPICFLAGS)
