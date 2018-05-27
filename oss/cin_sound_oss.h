/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_SOUND_OSS_H
#define CIN_SOUND_OSS_H
#pragma once

#include "dsp/cin_dsp.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Cin_Lock;
struct Cin_DriverMixer;

#define CIN_OSS_SOUND_STOP 0
#define CIN_OSS_SOUND_PLAY 1

struct Cin_OSS_SoundTableData {
    struct Cin_Lock *lock;
    int **table;
    unsigned offset;
    struct Cin_DriverMixer *mixer;
};

struct Cin_OSS_SoundData{
    struct Cin_DSP_MixerFormat format;
    unsigned num_bytes;
    void *samples;
};

struct Cin_Sound {
    struct Cin_OSS_SoundData sound_data;
    struct Cin_OSS_SoundTableData state_table_data;
};

void Cin_OSS_SetSoundState(struct Cin_OSS_SoundTableData *table, int state);

#define CIN_OSS_STOP(SND)\
    Cin_OSS_SetSoundState(&((SND)->state_table_data), CIN_OSS_SOUND_STOP)

#define CIN_OSS_PLAY(SND)\
    Cin_OSS_SetSoundState(&((SND)->state_table_data), CIN_OSS_SOUND_PLAY)

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_SOUND_OSS_H */
