/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_common.h"
#include "cin_loader.h"
#include "cin_driver.h"

#include <assert.h>
   
#define CIN_COMMON_LOADER_ASSERT_ENUMS_EQUAL(WHAT)\
    assert((int)Cin_eLoader ## WHAT == (int)Cin_eDriver ## WHAT)

CIN_PRIVATE(enum Cin_LoaderError) Cin_FormatCompatible(struct Cin_Driver *drv,
    unsigned sample_rate,
    unsigned num_channels,
    enum Cin_Format format){
    
    CIN_COMMON_LOADER_ASSERT_ENUMS_EQUAL(UnsupportedFormat);
    CIN_COMMON_LOADER_ASSERT_ENUMS_EQUAL(InvalidFormat);
    CIN_COMMON_LOADER_ASSERT_ENUMS_EQUAL(UnsupportedNumChannels);
    CIN_COMMON_LOADER_ASSERT_ENUMS_EQUAL(UnsupportedSampleRate);
    
    {
        const enum Cin_DriverError drv_err =
            Cin_DriverSupportsFormat(drv, format, num_channels);
        if(drv_err != Cin_eDriverSuccess){
            assert(drv_err == Cin_eDriverUnsupportedFormat ||
                drv_err == Cin_eDriverInvalidFormat ||
                drv_err == Cin_eDriverUnsupportedNumChannels);
            return (enum Cin_LoaderError)drv_err;
        }
    }
    
    {
        const enum Cin_DriverError drv_err =
            Cin_DriverSupportsSampleRate(drv, sample_rate);
        if(drv_err != Cin_eDriverSuccess){
            assert(drv_err == Cin_eDriverUnsupportedSampleRate);
            return (enum Cin_LoaderError)drv_err;
        }
    }
    
    return Cin_eLoaderSuccess;
}

CIN_PRIVATE_PURE(unsigned) Cin_BytesPerSample(enum Cin_Format format){
    switch(format){
        case Cin_eFormatS8:
            return 1;
        case Cin_eFormatS16:
            return 2;
        case Cin_eFormatS32:
            return 4;
        case Cin_eFormatFloat32:
            return 4;
        case Cin_eFormatFloat64:
            return 8;
        case Cin_eFormatULaw8:
            return 1;
        default:
            return 0;
    }
}
