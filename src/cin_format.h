/* Copyright (c) 2018-2019 Alaskan Emily, Transnat Games
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_FORMAT_H
#define CIN_FORMAT_H
#pragma once

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

#endif /* CIN_FORMAT_H */
