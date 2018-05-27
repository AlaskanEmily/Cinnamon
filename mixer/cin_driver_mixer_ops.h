/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_DRIVER_MIXER_OPS_H
#define CIN_DRIVER_MIXER_OPS_H
#pragma once

/* Provides some simple ops on a mixer driver to allow the sound and loader
 * implementations to be pure C.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Cin_DriverMixer;

struct Cin_Sound;

/* Lock and unlock should not be called surrounding AddSound/RemoveSound. */
void Cin_MixerDriverLock(struct Cin_DriverMixer *that);

void Cin_MixerDriverUnlock(struct Cin_DriverMixer *that);

void Cin_MixerDriverRemoveSound(struct Cin_DriverMixer *that,
    struct Cin_Sound *sound);

void Cin_MixerDriverAddSound(struct Cin_DriverMixer *that,
    struct Cin_Sound *sound,
    uintptr_t event);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_DRIVER_MIXER_OPS_H */
