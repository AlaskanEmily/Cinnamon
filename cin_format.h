/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
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

#endif /* CIN_FORMAT_H */
