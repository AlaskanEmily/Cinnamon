/* Copyright (c) 2019 Alaskan Emily, Transnat Games
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_mixer_sound.h"
#include "cin_soft_loader.h"
#include "cin_dsp.h"

/*****************************************************************************/

CIN_PRIVATE(void) Cin_ResampleSound(const struct Cin_Loader *from,
    unsigned target_sample_rate,
    unsigned target_channels,
    enum Cin_Format target_format,
    struct Cin_MixerSound *out_sound){
    
    unsigned at = 0;
    struct Cin_LoaderData *ld_data;
    CIN_DSP_NEW_STACK_FMT_STRUCT_N(formats, 2);
    struct Cin_DSP_MixerFormat *const in_format = formats+0;
    struct Cin_DSP_MixerFormat *const out_format = formats+1;
    
    CIN_DSP_MIXER_FORMAT_SET(in_format,
        from->format,
        from->sample_rate,
        from->channels);
    
    CIN_DSP_MIXER_FORMAT_SET(out_format,
        target_format,
        target_sample_rate,
        target_channels);
    
    for(ld_data = from->first;
        ld_data && at < from->bytes_placed;
        ld_data = ld_data->next){
        
        Cin_DSP_Convert(ld_data->len,
            in_format,
            CIN_SOFT_LOADER_DATA(ld_data),
            out_format,
            CIN_MIXER_SOUND_PCM(out_sound) + at);
        
        at += ld_data->len;
    }
    CIN_DSP_FREE_STACK_FMT_STRUCT(formats);
}

/*****************************************************************************/
