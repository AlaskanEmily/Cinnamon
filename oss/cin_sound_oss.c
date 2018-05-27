/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_sound.h"
#include "cin_sound_oss.h"

#include "cin_loader.h"
#include "cin_loader_soft.h"

#include "mixer/cin_lock.h"
#include "mixer/cin_driver_mixer_ops.h"

#include <stdlib.h>
#include <string.h>

void Cin_OSS_SetSoundState(struct Cin_OSS_SoundTableData *table, int state){
    Cin_ClaimLock(table->lock);
    table->table[0][table->offset] = state;
    Cin_ReleaseLock(table->lock);
}

enum Cin_LoaderError Cin_LoaderFinalize(struct Cin_Loader *ld,
    struct Cin_Sound *snd){
    
    memcpy(&(snd->state_table_data),
        ld->data,
        sizeof(struct Cin_OSS_SoundTableData));
    free(ld->data);
    
    {
        const unsigned num_bytes = ld->bytes_placed;
        void *const data = malloc(num_bytes);
        snd->sound_data.num_bytes = num_bytes;
        snd->sound_data.samples = data;
        Cin_LoaderMemcpy(ld->first, 0, data, num_bytes);
    }
    
    snd->sound_data.format.sample_rate = ld->sample_rate;
    snd->sound_data.format.num_channels = ld->channels;
    snd->sound_data.format.sample_format = ld->format;
    Cin_MixerDriverAddSound(snd->state_table_data.mixer, snd, 0);
    return Cin_eSoundSuccess;
}

unsigned Cin_StructSoundSize(){
    return sizeof(struct Cin_Sound);
}

enum Cin_SoundError Cin_SoundPlay(struct Cin_Sound *snd){
    Cin_OSS_SetSoundState(&(snd->state_table_data), CIN_OSS_SOUND_PLAY);
    return Cin_eSoundSuccess;
}

enum Cin_SoundError Cin_SoundStop(struct Cin_Sound *snd){
    Cin_OSS_SetSoundState(&(snd->state_table_data), CIN_OSS_SOUND_STOP);
    return Cin_eSoundSuccess;
}

void Cin_DestroySound(struct Cin_Sound *snd){
    Cin_MixerDriverRemoveSound(snd->state_table_data.mixer, snd);
}
