/* Copyright (c) 2019 Alaskan Emily, Transnat Games
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_MIXER_SOUND_H
#define CIN_MIXER_SOUND_H
#pragma once

#include "cin_export.h"
#include "cin_format.h"

/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

#ifdef _MSC_VER
/* Push the warning about the pcm[] field */
#pragma warning(push)
#pragma warning(disable: 4200)

#define CIN_MIXER_USE_PCM_FIELD

#elif ( __STDC_VERSION__ >= 199901L ) || ( __cplusplus >= 201103L )

#define CIN_MIXER_USE_PCM_FIELD

#endif

/*****************************************************************************/

struct Cin_MixerSound {
    unsigned byte_len, position;
#ifdef CIN_MIXER_USE_PCM_FIELD
    unsigned char pcm[];
#define CIN_MIXER_SOUND_PCM(SND) ((SND)->pcm)
#else
#define CIN_MIXER_SOUND_PCM(SND) ((unsigned char *)((SND)+1))
#endif

};

/*****************************************************************************/

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/*****************************************************************************/

typedef struct Cin_MixerSound *Cin_MixerSound_Ptr;
typedef const struct Cin_MixerSound *Cin_MixerSound_ConstPtr;

struct Cin_Loader;

/*****************************************************************************/

CIN_PRIVATE(void) Cin_ResampleSound(const struct Cin_Loader *from,
    unsigned target_sample_rate,
    unsigned target_channels,
    enum Cin_Format target_format,
    struct Cin_MixerSound *out_sound);

/*****************************************************************************/

/* Usable on backends that have mixer sounds and soft loaders (which is most).
 * You must include cin_soft_loader.h and cin_dsp.h to use this.
 * This is only a macro to allow us to avoid adding an additional library just
 * to glue together SoftLoader and MixerSound.
 * TODO: This just swallows errors!
 */
#define CIN_MIXER_SOUND_FROM_SOFT_LOADER(LD, FMT, OUT, ALLOCATOR) do{ \
        const struct Cin_Loader *const CIN_MIXER_loader = (LD);\
        const struct Cin_DSP_MixerFormat *const CIN_MIXER_out_format = (FMT); \
        CIN_DSP_NEW_STACK_FMT_STRUCT(CIN_MIXER_in_format); \
        \
        CIN_DSP_MIXER_FORMAT_SET(CIN_MIXER_in_format, \
            CIN_MIXER_loader->format, \
            CIN_MIXER_loader->sample_rate, \
            CIN_MIXER_loader->channels); \
         \
        { \
            const struct Cin_LoaderData *CIN_MIXER_loader_data; \
            unsigned CIN_MIXER_i = 0;\
            const unsigned CIN_MIXER_out_size = Cin_DSP_ConversionSize( \
                CIN_MIXER_loader->bytes_placed, \
                CIN_MIXER_in_format, \
                CIN_MIXER_out_format); \
            \
            struct Cin_MixerSound *const CIN_MIXER_out = \
                ALLOCATOR(sizeof(struct Cin_MixerSound) + CIN_MIXER_out_size); \
            (OUT) = CIN_MIXER_out; \
            CIN_MIXER_out->byte_len = CIN_MIXER_out_size; \
            for(CIN_MIXER_loader_data = CIN_MIXER_loader->first; \
                CIN_MIXER_loader_data != NULL; \
                CIN_MIXER_loader_data = CIN_MIXER_loader_data->next){ \
                \
                const unsigned CIN_MIXER_added = Cin_DSP_Convert( \
                    CIN_MIXER_loader_data->len, \
                    CIN_MIXER_in_format, \
                    CIN_SOFT_LOADER_DATA(CIN_MIXER_loader_data), \
                    CIN_MIXER_out_format, \
                    CIN_MIXER_SOUND_PCM(CIN_MIXER_out) + CIN_MIXER_i); \
                if(CIN_MIXER_added == 0) break; \
                CIN_MIXER_i += CIN_MIXER_added; \
            } \
        } \
        CIN_DSP_FREE_STACK_FMT_STRUCT(CIN_MIXER_in_format); \
    } while(0)

/*****************************************************************************/
#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_MIXER_SOUND_H */
