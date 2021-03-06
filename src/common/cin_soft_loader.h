/* Copyright (c) 2018 AlaskanEmily
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_LOADER_SOFT_H
#define CIN_LOADER_SOFT_H
#pragma once

#include "cin_export.h"
#include "cin_format.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Implementation of a loader in software only. */

#ifdef _MSC_VER
/* Push the warning about the data[] field */
#pragma warning(push)
#pragma warning(disable: 4200)

#define CIN_SOFT_USE_DATA_FIELD

#elif ( __STDC_VERSION__ >= 199901L ) || ( __cplusplus >= 201103L )

#define CIN_SOFT_USE_DATA_FIELD

#endif

/*****************************************************************************/

struct Cin_LoaderData {
    struct Cin_LoaderData *next;
    unsigned len;
#ifdef CIN_SOFT_USE_DATA_FIELD
    unsigned char data[];
#define CIN_SOFT_LOADER_DATA(LD) ((LD)->data)
#else
#define CIN_SOFT_LOADER_DATA(LD) ((unsigned char *)((LD)+1))
#endif
};

/*****************************************************************************/

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/*****************************************************************************/

struct Cin_Loader{
    unsigned sample_rate;
    unsigned channels;
    enum Cin_Format format;
    struct Cin_LoaderData *first, *last;
    unsigned bytes_placed;
    void *data; /**< Data placed here by the backend. */
};

/*****************************************************************************/

CIN_PRIVATE(void) Cin_LoaderFreeData(struct Cin_LoaderData *data);

/*****************************************************************************/

CIN_PRIVATE(void) Cin_LoaderMemcpy(const struct Cin_LoaderData *data,
    unsigned at,
    void *dest,
    unsigned count);

/*****************************************************************************/

CIN_PRIVATE(void) Cin_CreateSoftLoader(struct Cin_Loader *out,
    unsigned sample_rate,
    unsigned channels,
    enum Cin_Format format);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_LOADER_SOFT_H */
