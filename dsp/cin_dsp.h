/*
 *  Cinnamon DSP/Mixer Code
 *
 *  Copyright (C) 2018 AlaskanEmily
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  - The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *  - Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *  - This notice may not be removed or altered from any source distribution.
 */

#ifndef CIN_DSP_H
#define CIN_DSP_H
#pragma once

#ifdef CIN_DSP_USE_EXPORT
    #include "cin_export.h"
    #define CIN_DSP_EXPORT CIN_EXPORT
#else
    #define CIN_DSP_EXPORT(X) X
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* This MUST match up with Cinnamon's cin_format.h */
#define CIN_DSP_FORMAT_S8 0
#define CIN_DSP_FORMAT_S16 1
#define CIN_DSP_FORMAT_S32 2
#define CIN_DSP_FORMAT_FLOAT32 3
#define CIN_DSP_FORMAT_FLOAT64 4
#define CIN_DSP_FORMAT_ULAW 5
#define CIN_DSP_FORMAT_NUM_FORMATS 6

#ifndef CIN_DSP_HIDE_STRUCTS

struct Cin_DSP_MixerFormat{
    unsigned sample_rate;
    unsigned char num_channels;
    
    /** See the CIN_DSP_FORMAT_* defines. */
    unsigned char sample_format;
};

#endif /* CIN_DSP_HIDE_STRUCTS */

/**
 * @defgroup CinDSPCompilerCompat Cross-Compiler compatibility functions
 * @{
 */

/**
 * @brief Returns the size needed for a Cin_DSP_MixerFormat
 */
CIN_DSP_EXPORT(unsigned) Cin_DSP_MixerFormatStructSize();

/**
 * @brief Sets all members of a Cin_DSP_MixerFormat
 */
CIN_DSP_EXPORT(void) Cin_DSP_MixerFormatSet(struct Cin_DSP_MixerFormat *format,
    unsigned sample_format,
    unsigned sample_rate,
    unsigned num_channels);

/**
 * @brief Gets the sample format for a Cin_DSP_MixerFormat
 *
 * Returns a Cin_Format enum value.
 *
 * @sa Cin_DSP_MixerFormatSetFormat
 */
CIN_DSP_EXPORT(unsigned) Cin_DSP_MixerFormatGetFormat(
    const struct Cin_DSP_MixerFormat *format);

/**
 * @brief Sets the sample format for a Cin_DSP_MixerFormat
 *
 * @sa Cin_DSP_MixerFormatGetFormat
 */
CIN_DSP_EXPORT(void) Cin_DSP_MixerFormatSetFormat(
    struct Cin_DSP_MixerFormat *format,
    unsigned sample_format);

/**
 * @brief Gets the sample rate (in Hz) for a Cin_DSP_MixerFormat
 */
CIN_DSP_EXPORT(unsigned) Cin_DSP_MixerFormatGetSampleRate(
    const struct Cin_DSP_MixerFormat *format);

/**
 * @brief Sets the sample rate (in Hz) for a Cin_DSP_MixerFormat
 */
CIN_DSP_EXPORT(void) Cin_DSP_MixerFormatSetSampleRate(
    struct Cin_DSP_MixerFormat *format,
    unsigned rate);

/**
 * @brief Gets the number of channels for a Cin_DSP_MixerFormat
 */
CIN_DSP_EXPORT(unsigned) Cin_DSP_MixerFormatGetNumChannels(
    const struct Cin_DSP_MixerFormat *format);

/**
 * @brief Sets the sample format for a Cin_DSP_MixerFormat
 */
CIN_DSP_EXPORT(void) Cin_DSP_MixerFormatSetNumChannels(
    struct Cin_DSP_MixerFormat *format,
    unsigned channels);

/**
 * @}
 */

/**
 * @brief Calculates the size of the required buffer for a conversion.
 *
 * Calculates how many bytes are needed to convert @p num_bytes in the source
 * format specified by @p in_format to the destination format of @p out_format.
 */
CIN_DSP_EXPORT(unsigned) Cin_DSP_ConversionSize(unsigned num_bytes,
    const struct Cin_DSP_MixerFormat *in_format,
    const struct Cin_DSP_MixerFormat *out_format);

/**
 * @brief Converts samples from one format to another
 *
 * Converts @p in_bytes of data from @p in_data from @p in_format format to
 * @p out_format format, and writes the output to @p out_data.
 *
 * @p out_data should have been created to be at least the number of bytes that
 * Cin_DSP_ConversionSize requires for the formats.
 */
CIN_DSP_EXPORT(unsigned) Cin_DSP_Convert(unsigned in_bytes,
    const struct Cin_DSP_MixerFormat *in_format,
    const void *in_data,
    const struct Cin_DSP_MixerFormat *out_format,
    void *out_data);

CIN_DSP_EXPORT(unsigned) Cin_DSP_Mix(unsigned num_bytes,
    const struct Cin_DSP_MixerFormat *format,
    const void *in_data0,
    const void *in_data1,
    void *out_data);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_DSP_H */
