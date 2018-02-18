/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifdef __APPLE__
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#else
    #include <AL/al.h>
    #include <AL/alc.h>
#endif

#include "cin_export.h"
#include "cin_format.h"

#ifndef AL_FORMAT_MONO_FLOAT32
#define AL_FORMAT_MONO_FLOAT32 0x10010
#endif

#ifndef AL_FORMAT_STEREO_FLOAT32
#define AL_FORMAT_STEREO_FLOAT32 0x10011
#endif

#ifndef AL_FORMAT_MONO_DOUBLE_EXT
#define AL_FORMAT_MONO_DOUBLE_EXT 0x10012
#endif

#ifndef AL_FORMAT_STEREO_DOUBLE_EXT
#define AL_FORMAT_STEREO_DOUBLE_EXT 0x10013
#endif

#ifndef AL_FORMAT_MONO_MULAW_EXT
#define AL_FORMAT_MONO_MULAW_EXT 0x10014
#endif

#ifndef AL_FORMAT_STEREO_MULAW_EXT
#define AL_FORMAT_STEREO_MULAW_EXT 0x10015
#endif

#ifndef AL_FORMAT_QUAD_MULAW
#define AL_FORMAT_QUAD_MULAW 0x10021
#endif

#ifndef AL_FORMAT_QUAD_MULAW_EXT
#define AL_FORMAT_QUAD_MULAW_EXT AL_FORMAT_QUAD_MULAW
#endif

#ifndef AL_FORMAT_QUAD8
#define AL_FORMAT_QUAD8 0x1204
#endif

#ifndef AL_FORMAT_QUAD16
#define AL_FORMAT_QUAD16 0x1205
#endif

#ifdef __cplusplus
extern "C"
#endif
CIN_PRIVATE_PURE(ALuint)
    Cin_CinFormatToOpenALFormat(enum Cin_Format f, unsigned num_channels);

/* Defines the bits in exts that indicate presence. */
#define CIN_FLOAT_SUPPORTED 1
#define CIN_DOUBLE_SUPPORTED 2
#define CIN_MULAW_SUPPORTED 3
#define CIN_MULAW_QUAD_SUPPORTED 4
#define CIN_QUAD_SUPPORTED 5

CIN_PRIVATE(int) Cin_CinExtension(const struct Cin_Driver *drv, int ext);

CIN_PRIVATE(int) Cin_CleanOpenALSound(ALuint snd, ALuint *out);

struct Cin_Driver{
    ALCcontext *ctx;
    ALCdevice *dev;
    /* See the CIN_*_SUPPORTED macros.
     * The LSB indicates if this has been set. */
    short ext;
};

struct Cin_Sound {
    ALCcontext *ctx;
    ALuint snd;
};

struct Cin_Loader {
    struct Cin_Sound snd;
    ALuint format, sample_rate, channels;
};
