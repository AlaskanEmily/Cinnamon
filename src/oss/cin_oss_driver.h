/* Copyright (c) 2019 Alaskan Emily, Transnat Games
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_OSS_DRIVER_H
#define CIN_OSS_DRIVER_H
#pragma once

#include "cin_export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CIN_OSS_SOUND_CHANNELS 16

#define CIN_OSS_INSERT 0
#define CIN_OSS_DELETE 1
#define CIN_OSS_PLAY 2
#define CIN_OSS_STOP 3
#define CIN_OSS_QUIT 0x80

/*****************************************************************************/

struct Cin_MixerSound;
typedef struct Cin_MixerSound **Cin_MixerSound_PtrPtr;

/*****************************************************************************/

struct Cin_OSS_Command;
typedef struct Cin_OSS_Command *Cin_OSS_Command_Ptr;
typedef const struct Cin_OSS_Command *Cin_OSS_Command_ConstPtr;

/*****************************************************************************/

struct Cin_Driver;

/*****************************************************************************/

CIN_PRIVATE(Cin_OSS_Command_Ptr) Cin_OSS_NewCommand(int id, int cmd);
CIN_PRIVATE(Cin_OSS_Command_Ptr) Cin_OSS_CommandInsertSound(
    struct Cin_OSS_Command *cmd,
    struct Cin_MixerSound *snd);

#define CIN_OSS_INSERT_SOUND_CMD(ID, SND) \
    Cin_OSS_CommandInsertSound(Cin_OSS_NewCommand((ID), CIN_OSS_INSERT), (SND))

/*****************************************************************************/

CIN_PRIVATE(int) Cin_OSS_Lock(struct Cin_Driver *);

/*****************************************************************************/

CIN_PRIVATE(int) Cin_OSS_Unlock(struct Cin_Driver *);

/*****************************************************************************/

CIN_PRIVATE(int) Cin_OSS_NewID(struct Cin_Driver *);

/*****************************************************************************/

CIN_PRIVATE(int) Cin_OSS_PushCommand(struct Cin_Driver *,
    struct Cin_OSS_Command *);

/*****************************************************************************/
/* Used by the the thread backend to play channels. */
CIN_PRIVATE_PURE(Cin_MixerSound_PtrPtr) Cin_OSS_GetChannels(
    const struct Cin_Driver *);

/*****************************************************************************/

CIN_PRIVATE_PURE(int) Cin_OSS_GetDevice(const struct Cin_Driver *);

/*****************************************************************************/
/* Does not require locking. */
CIN_PRIVATE_PURE(unsigned) Cin_OSS_GetSampleRate(const struct Cin_Driver *);

/*****************************************************************************/
/* Does not require locking. */
CIN_PRIVATE_PURE(int) Cin_OSS_GetNumChannels(const struct Cin_Driver *);

/*****************************************************************************/
/* Does not require locking. */
CIN_PRIVATE_PURE(int) Cin_OSS_GetFormat(const struct Cin_Driver *);

/*****************************************************************************/
/* As with all thread functions, the driver must NOT be locked when called.
 * returns 1 if the calling thread should die.
 */
CIN_PRIVATE(int) Cin_OSS_ProcessCommands(struct Cin_Driver *);

/*****************************************************************************/

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_OSS_DRIVER_H */
