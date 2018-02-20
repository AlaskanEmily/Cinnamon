/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_LOADER_H
#define CIN_LOADER_H
#pragma once

#include "cin_export.h"
#include "cin_format.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Cin_Driver;

struct Cin_Sound;

/**
 * @defgroup Loader Functions that play, stop, and destroy Loaders.
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

#endif /* CIN_LOADER_H */
