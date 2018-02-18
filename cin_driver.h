/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_DRIVER_H
#define CIN_DRIVER_H
#pragma once

#include "cin_export.h"
#include "cin_format.h"

#ifdef __cplusplus
extern "C" {
#endif


struct Cin_Loader;
struct Cin_Sound;

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

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_DRIVER_H */
