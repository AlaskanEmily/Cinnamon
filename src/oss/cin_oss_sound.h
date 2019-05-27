/* Copyright (c) 2019 Alaskan Emily, Transnat Games
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_OSS_SOUND_H
#define CIN_OSS_SOUND_H
#pragma once

#include "cin_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

struct Cin_Driver;

/*****************************************************************************/

struct Cin_MixerSound;

/*****************************************************************************/

struct Cin_Sound {
    struct Cin_MixerSound *snd;
    struct Cin_Driver *drv;
    unsigned short sound_id;
};

/*****************************************************************************/

typedef struct Cin_Sound *Cin_Sound_Ptr;
typedef const struct Cin_Sound *Cin_Sound_ConstPtr;

/*****************************************************************************/

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_OSS_SOUND_H */
