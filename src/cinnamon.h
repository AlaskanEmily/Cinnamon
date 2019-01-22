/* Copyright (c) 2018-2019 Alaskan Emily, Transnat Games
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CINNAMON_H
#define CINNAMON_H
#pragma once

#include "cin_export.h"

#ifdef __cplusplus
extern "C" {
#endif

enum Cin_Format{
    Cin_eFormatS8,
    Cin_eFormatS16,
    Cin_eFormatS32,
    Cin_eFormatFloat32,
    Cin_eFormatFloat64,
    Cin_eFormatULaw8,
    Cin_eFormatNUM_FORMATS,
    
    Cin_eFormatBEGIN = 0,
    Cin_eFormatEND = (Cin_eFormatNUM_FORMATS - 1)
};

#define CIN_FORMAT_BYTES_PER_SAMPLE(FMT) ( \
    ((FMT) == Cin_eFormatS8 || (FMT) == Cin_eFormatULaw8) ? 1 : \
    ((FMT) == Cin_eFormatS16) ? 2 : \
    ((FMT) == Cin_eFormatS32 || (FMT) == Cin_eFormatFloat32) ? 4 : \
    ((FMT) == Cin_eFormatFloat64) ? 8 : 0 \
    )

/**
 * @defgroup Driver Functions that create, destroy, and manipulate the Driver.
 * @{
 */

struct Cin_Driver;

/**
 * @brief Errors that occur when creating or manipulating a Driver.
 */
enum Cin_DriverError {
    Cin_eDriverSuccess, /**< No error occured */
    Cin_eDriverFailure,
    Cin_eDriverUnsupportedFormat, /**< The specified format is not supported */
    Cin_eDriverInvalidFormat, /**< The specified format is invalid */
    Cin_eDriverUnsupportedNumChannels, /**< Unsupported number of channels */
    Cin_eDriverUnsupportedSampleRate, /**< Unsupported sample rate */
    Cin_eDriverNoDevice
};

/**
 * @brief Returns the size of struct Cin_Driver
 *
 * The client is expected to allocate space for the driver.
 */
CIN_EXPORT(unsigned) Cin_StructDriverSize(void);

/**
 * @brief Initializes a Driver.
 *
 * The client is expected to allocate space for the driver. Use
 * Cin_StructDriverSize to get the size of the driver struct.
 *
 * @warning The data that is placed in drv must NOT be copied. Do not use
 *   memcpy on @p sdrv, other structures may depend on the address of @p drv.
 *
 * @sa Cin_StructDriverSize
 * @sa Cin_DestroyDriver
 * @todo There is currently no way to enumerate devices.
 */
CIN_EXPORT(enum Cin_DriverError) Cin_CreateDriver(struct Cin_Driver *drv);

/**
 * @brief Destroys the Driver
 *
 * As the storage space for @p drv was allocated by the client, they must also
 * free the space after use.
 */
CIN_EXPORT(void) Cin_DestroyDriver(struct Cin_Driver *drv);

/**
 * @brief Returns if the Driver supports creating loaders for a format.
 */
CIN_EXPORT(enum Cin_DriverError) Cin_DriverSupportsFormat(
    const struct Cin_Driver *drv,
    enum Cin_Format format,
    unsigned num_channels);

CIN_EXPORT(enum Cin_DriverError) Cin_DriverSupportsSampleRate(
    const struct Cin_Driver *drv,
    unsigned rate);

/** @} */ /* End Driver group. */

/**
 * @defgroup Sound Functions that play, stop, and destroy Sounds.
 * @{
 */

struct Cin_Sound;

/**
 * @brief Errors that occur when manipulating a Sound.
 */
enum Cin_SoundError {
    Cin_eSoundSuccess, /**< No error occured */
    Cin_eSoundFailure
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

/**
 * @defgroup Loader Functions that create, put data on, and finalize Loaders.
 * @{
 */

struct Cin_Loader;

/**
 * @brief Errors that occur when manipulating a Loader.
 */
enum Cin_LoaderError {
    Cin_eLoaderSuccess, /**< No error occured */
    Cin_eLoaderFailure, /**< An unspecified error occured */
    Cin_eLoaderUnsupportedFormat, /**< The specified format is not supported */
    Cin_eLoaderInvalidFormat, /**< The specified format is invalid */
    Cin_eLoaderUnsupportedNumChannels, /**< Unsupported number of channels */
    Cin_eLoaderUnsupportedSampleRate /**< Unsupported sample rate */
};

/**
 * @brief Returns the size of struct Cin_Loader
 *
 * The client is expected to allocate space for the Loader.
 */
CIN_EXPORT(unsigned) Cin_StructLoaderSize();

CIN_EXPORT(enum Cin_LoaderError) Cin_CreateLoader(struct Cin_Loader *out,
    struct Cin_Driver *drv,
    unsigned sample_rate,
    unsigned channels,
    enum Cin_Format format);

CIN_EXPORT(enum Cin_LoaderError) Cin_LoaderPut(struct Cin_Loader *ld,
    const void *data,
    unsigned byte_size);

CIN_EXPORT(enum Cin_LoaderError) Cin_LoaderFinalize(struct Cin_Loader *ld,
    struct Cin_Sound *out);

/** @} */ /* End Loader group. */
#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_SOUND_H */
