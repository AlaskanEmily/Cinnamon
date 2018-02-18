/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_SOUND_H
#define CIN_SOUND_H
#pragma once

#include "cin_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Sound Functions that play, stop, and destroy Sounds.
 * @{
 */

struct Cin_Sound;

/**
 * @brief Errors that occur when manipulating a Sound.
 */
enum Cin_SoundError {
    Cin_eSoundSuccess /**< No error occured */
};

/**
 * @brief Returns the size of struct Cin_Sound
 *
 * The client is expected to allocate space for the Sound.
 */
CIN_EXPORT(unsigned) Cin_StructSoundSize();

CIN_EXPORT(enum Cin_SoundError) Cin_SoundPlay(struct Cin_Sound *snd);

CIN_EXPORT(enum Cin_SoundError) Cin_SoundStop(struct Cin_Sound *snd);

CIN_EXPORT(void) Cin_DestroySound(struct Cin_Sound *snd);

/** @} */ /* End Sound group. */

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_SOUND_H */
