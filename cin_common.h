/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_COMMON_H
#define CIN_COMMON_H
#pragma once

#include "cin_export.h"
#include "cin_format.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Cin_Driver;
struct Cin_Loader;
struct Cin_Sound;

CIN_PRIVATE(enum Cin_LoaderError) Cin_FormatCompatible(struct Cin_Driver *drv,
    unsigned sample_rate,
    unsigned num_channels,
    enum Cin_Format format);

CIN_PRIVATE_PURE(unsigned) Cin_BytesPerSample(enum Cin_Format format);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_COMMON_H */
