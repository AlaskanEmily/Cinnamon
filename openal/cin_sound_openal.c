/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_sound.h"
#include "cin_openal.h"

unsigned Cin_StructSoundSize(){
    return sizeof(struct Cin_Sound);
}

enum Cin_SoundError Cin_SoundPlay(struct Cin_Sound *snd){
    alcMakeContextCurrent(snd->ctx);
    alSourcePlay(snd->snd);
    return Cin_eSoundSuccess;
}

enum Cin_SoundError Cin_SoundStop(struct Cin_Sound *snd){
    alcMakeContextCurrent(snd->ctx);
    alSourceStop(snd->snd);
    alSourceRewind(snd->snd);
    return Cin_eSoundSuccess;
}

void Cin_DestroySound(struct Cin_Sound *snd){
    alcMakeContextCurrent(snd->ctx);
    alDeleteSources(1, &(snd->snd));
}
