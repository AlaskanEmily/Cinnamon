/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "cin_driver_mixer_ops.h"
#include "cin_driver_mixer.hpp"

void Cin_MixerDriverLock(struct Cin_DriverMixer *that){
    that->lock();
}

void Cin_MixerDriverUnlock(struct Cin_DriverMixer *that){
    that->unlock();
}

void Cin_MixerDriverRemoveSound(struct Cin_DriverMixer *that,
    struct Cin_Sound *sound){
    that->removeSound(sound);
}

void Cin_MixerDriverAddSound(struct Cin_DriverMixer *that,
    struct Cin_Sound *sound,
    uintptr_t event){
    that->addSound(sound, event);
}
