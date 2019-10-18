/*
 *  Cinnamon DSP/Mixer Code
 *
 *  Copyright (C) 2018-2019 AlaskanEmily
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

/*****************************************************************************/
/* NOTE:
 * This is VERY prototype mixer stuff. It has some really bad problems when
 * resampling to very similar frequencies. 48000 to 44100 works OK.
 *
 * We probably want to replace this with the mixer code from sndiod later.
 */

#ifdef _MSC_VER
#define CIN_DSP_CALL(T) __declspec(noalias) T cdecl
#elif defined __WATCOMC__
#define CIN_DSP_CALL(T) T __cdecl
#elif defined __GNUC__
#define CIN_DSP_CALL(T) __attribute__((cdecl,nothrow,nonnull)) T
#else
#define CIN_DSP_CALL(T) T
#endif

/*****************************************************************************/

#if defined __GNUC__
#define CIN_DSP_GETTER(T) __attribute__((cdecl,nothrow,warn_unused_result,nonnull)) T
#define CIN_DSP_SETTER(T) __attribute__((cdecl,nothrow,nonnull)) T
#define CIN_DSP_PURE(T) __attribute__((cdecl,nothrow,const,warn_unused_result)) T
#else
#define CIN_DSP_GETTER CIN_DSP_CALL
#define CIN_DSP_SETTER CIN_DSP_CALL
#define CIN_DSP_PURE CIN_DSP_CALL
#endif

/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

/* This MUST match up with Cinnamon's cin_format.h */
#define CIN_DSP_FORMAT_S8 0
#define CIN_DSP_FORMAT_S16 1
#define CIN_DSP_FORMAT_S32 2
#define CIN_DSP_FORMAT_FLOAT32 3
#define CIN_DSP_FORMAT_FLOAT64 4
#define CIN_DSP_FORMAT_ULAW 5
#define CIN_DSP_FORMAT_NUM_FORMATS 6

/**
 * @defgroup DSPFormatStruct Macros to manipulate format structs.
 * @{
 */

/**
 * @def CIN_DSP_NEW_STACK_FMT_STRUCT(name)
 *
 * @brief Defines a Cin_DSP_MixerFormat with @a name.
 *
 * Defines a Cin_DSP_MixerFormat on the stack if CIN_DSP_HIDE_STRUCTS is unset
 * or if CIN_DSP_USE_ALLOCA is set (DO NOT DO THIS FROM MERCURY!), otherwise
 * uses malloc/free.
 *
 * @warning Always pair this with CIN_DSP_FREE_STACK_FMT_STRUCT
 *
 * @sa CIN_DSP_NEW_STACK_FMT_STRUCT_N
 * @sa CIN_DSP_FREE_STACK_FMT_STRUCT
 */

/**
 * @def CIN_DSP_NEW_STACK_FMT_STRUCT_N(name, n)
 *
 * @brief Defines an array of @a n Cin_DSP_MixerFormat structs with @a name.
 *
 * @warning Always pair this with CIN_DSP_FREE_STACK_FMT_STRUCT
 *
 * @sa CIN_DSP_NEW_STACK_FMT_STRUCT
 * @sa CIN_DSP_FREE_STACK_FMT_STRUCT
 */

/**
 * @def CIN_DSP_FREE_STACK_FMT_STRUCT(name)
 *
 * @brief Frees the result of CIN_DSP_NEW_STACK_FMT_STRUCT
 *
 * This is a no-op if CIN_DSP_HIDE_STRUCTS is unset or if CIN_DSP_USE_ALLOCA is
 * set.
 */

/**
 * @def CIN_DSP_MIXER_FORMAT_STRUCT_SIZE()
 * @return Size of a struct Cin_DSP_MixerFormat
 */
 
/**
 * @def CIN_DSP_MIXER_FORMAT_SET(dst, format, rate, nchan)
 *
 * @brief Sets a MixerFormat with one call.
 *
 * This will be more efficient than setting each member individually when
 * CIN_DSP_HIDE_STRUCTS is set.
 *
 * @param dst MixerFormat to set.
 * @param format Format for @a dst.
 * @param rate Sample rate @a dst.
 * @param nchan Number of channels for @a dst.
 *
 * @sa CIN_DSP_MIXER_FORMAT_SET_FORMAT
 * @sa CIN_DSP_MIXER_FORMAT_SET_SAMPLE_RATE
 * @sa CIN_DSP_MIXER_FORMAT_SET_NUM_CHANNELS
 */

/**
 * @def CIN_DSP_MIXER_FORMAT_SET_FORMAT(dst, fmt)
 * @brief Sets the format on the MixerFormat @a dst to @a fmt.
 */

/**
 * @def CIN_DSP_MIXER_FORMAT_GET_FORMAT(dst)
 * @return format of the MixerFormat @a dst.
 */

/**
 * @def CIN_DSP_MIXER_FORMAT_SET_SAMPLE_RATE(dst, rate)
 * @brief Sets the sample rate on the MixerFormat @a dst to @a rate.
 */

/**
 * @def CIN_DSP_MIXER_FORMAT_GET_SAMPLE_RATE(dst)
 * @return Sample rate of the MixerFormat @a dst.
 */

/**
 * @def CIN_DSP_MIXER_FORMAT_SET_NUM_CHANNELS(dst, nchan)
 * @brief Sets the number of channels on the MixerFormat @a dst to @a nchan.
 */

/**
 * @def CIN_DSP_MIXER_FORMAT_GET_NUM_CHANNELS(dst)
 * @return number of channels of the MixerFormat @a dst.
 */

/** @} */ /* End Cin_DSP_MixerFormat group. */

/*****************************************************************************/

#ifndef CIN_DSP_HIDE_STRUCTS

/*****************************************************************************/

struct Cin_DSP_MixerFormat{
    unsigned sample_rate;
    unsigned char num_channels;
    
    /** See the CIN_DSP_FORMAT_* defines. */
    unsigned char sample_format;
};
 
/*****************************************************************************/

#define CIN_DSP_NEW_STACK_FMT_STRUCT(NAME) struct Cin_DSP_MixerFormat NAME[1]
#define CIN_DSP_NEW_STACK_FMT_STRUCT_N(NAME, N) \
    struct Cin_DSP_MixerFormat NAME[(N)]
#define CIN_DSP_FREE_STACK_FMT_STRUCT(X) ((void)X)

/*****************************************************************************/

#define CIN_DSP_MIXER_FORMAT_STRUCT_SIZE() sizeof(struct Cin_DSP_MixerFormat)

/*****************************************************************************/

#define CIN_DSP_MIXER_FORMAT_SET(TO, FMT, RATE, NCHAN) do{ \
        struct Cin_DSP_MixerFormat *CIN_TO = (TO);\
        CIN_TO->sample_rate = (RATE);\
        CIN_TO->num_channels = (NCHAN);\
        CIN_TO->sample_format = (FMT);\
    }while(0)

/*****************************************************************************/

#define CIN_DSP_MIXER_FORMAT_GET_FORMAT(FROM) ((FROM)->sample_format)

/*****************************************************************************/

#define CIN_DSP_MIXER_FORMAT_SET_FORMAT(TO, FMT) \
    ((void)((TO)->sample_format = (FMT)))

/*****************************************************************************/

#define CIN_DSP_MIXER_FORMAT_GET_SAMPLE_RATE(FROM) ((FROM)->sample_rate)

/*****************************************************************************/

#define CIN_DSP_MIXER_FORMAT_SET_SAMPLE_RATE(TO, RATE) \
    ((void)((TO)->sample_rate = (RATE)))

/*****************************************************************************/

#define CIN_DSP_MIXER_FORMAT_GET_NUM_CHANNELS(FROM) ((FROM)->num_channels)

/*****************************************************************************/

#define CIN_DSP_MIXER_FORMAT_SET_NUM_CHANNELS(TO, NCHAN) \
    ((void)((TO)->num_channels = (NCHAN)))

/*****************************************************************************/

#else /* CIN_DSP_HIDE_STRUCTS */

/*****************************************************************************/
/* Used by the allocator checkers */
#define CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, N, ALLOCATOR) \
    register struct Cin_DSP_MixerFormat *const NAME = \
    ALLOCATOR(Cin_DSP_MixerFormatStructSize() * (N))

#ifdef MR_GC_NEW /* Used in the case of Mercury code. */

#define CIN_DSP_NEW_STACK_FMT_STRUCT(NAME) \
    CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, 1, MR_GC_malloc_atomic)

#define CIN_DSP_NEW_STACK_FMT_STRUCT_N(NAME, N) \
    CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, N, MR_GC_malloc_atomic)

#define CIN_DSP_FREE_STACK_FMT_STRUCT MR_GC_free

#elif (defined CIN_DSP_USE_ALLOCA) /* Force use of alloca. */

    /* Various alloca's */
    #ifdef _MSC_VER
        /* Use Visual C++ black magic with _malloca and __if_exists. */
        #include <malloc.h>
        
        #define CIN_DSP_NEW_STACK_FMT_STRUCT(NAME) \
            CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, 1, alloca)
        
        #define CIN_DSP_NEW_STACK_FMT_STRUCT_N(NAME, N) \
            CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, N, _malloca); \
            void *const NAME ## _CIN_DSP_secret = (NAME)
        
        #define CIN_DSP_FREE_STACK_FMT_STRUCT(X) \
            __if_exists( NAME ## _CIN_DSP_secret ) { _freea(NAME); }
        
    #elif defined __GNUC__
        /* Use the builtin for alloca. This also avoids an extra include. */
        #define CIN_DSP_NEW_STACK_FMT_STRUCT(NAME) \
            CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, 1, __builtin_alloca)
        
        #define CIN_DSP_NEW_STACK_FMT_STRUCT_N(NAME, N) \
            CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, N, __builtin_alloca)
        
        #define CIN_DSP_FREE_STACK_FMT_STRUCT(X) ((void)(X))
        
    #else
        #ifdef __WATCOMC__
            #include <alloca.h>
        #else
            #warning Please update this to have your compiler!
            #include <stdlib.h>
        #endif
        #define CIN_DSP_NEW_STACK_FMT_STRUCT(NAME) \
            CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, 1, alloca)

        #define CIN_DSP_NEW_STACK_FMT_STRUCT_N(NAME, N) \
            CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, N, alloca)

        #define CIN_DSP_FREE_STACK_FMT_STRUCT(X) ((void)(X))
    
    #endif

#else /* Not alloca */

#define CIN_DSP_NEW_STACK_FMT_STRUCT(NAME) \
    CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, 1, malloc)

#define CIN_DSP_NEW_STACK_FMT_STRUCT_N(NAME, N) \
    CIN_DSP_NEW_STACK_FMT_STRUCT_HIDDEN(NAME, N, malloc)

#define CIN_DSP_FREE_STACK_FMT_STRUCT free

#endif

/*****************************************************************************/

#define CIN_DSP_MIXER_FORMAT_STRUCT_SIZE Cin_DSP_MixerFormatStructSize
#define CIN_DSP_MIXER_FORMAT_SET Cin_DSP_MixerFormatSet
#define CIN_DSP_MIXER_FORMAT_GET_FORMAT Cin_DSP_MixerFormatGetFormat
#define CIN_DSP_MIXER_FORMAT_SET_FORMAT Cin_DSP_MixerFormatSetFormat
#define CIN_DSP_MIXER_FORMAT_GET_SAMPLE_RATE Cin_DSP_MixerFormatGetSampleRate
#define CIN_DSP_MIXER_FORMAT_SET_SAMPLE_RATE Cin_DSP_MixerFormatSetSampleRate
#define CIN_DSP_MIXER_FORMAT_GET_NUM_CHANNELS Cin_DSP_MixerFormatGetNumChannels
#define CIN_DSP_MIXER_FORMAT_SET_NUM_CHANNELS Cin_DSP_MixerFormatSetNumChannels

/*****************************************************************************/

#endif /* CIN_DSP_HIDE_STRUCTS */

/**
 * @defgroup CinDSPCompilerCompat Cross-Compiler compatibility functions
 * @{
 */

/**
 * @brief Returns the size needed for a Cin_DSP_MixerFormat
 */
CIN_DSP_PURE(unsigned) Cin_DSP_MixerFormatStructSize();

/**
 * @brief Sets all members of a Cin_DSP_MixerFormat
 */
CIN_DSP_CALL(void) Cin_DSP_MixerFormatSet(struct Cin_DSP_MixerFormat *format,
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
CIN_DSP_GETTER(unsigned) Cin_DSP_MixerFormatGetFormat(
    const struct Cin_DSP_MixerFormat *format);

/**
 * @brief Sets the sample format for a Cin_DSP_MixerFormat
 *
 * @sa Cin_DSP_MixerFormatGetFormat
 */
CIN_DSP_SETTER(void) Cin_DSP_MixerFormatSetFormat(
    struct Cin_DSP_MixerFormat *format,
    unsigned sample_format);

/**
 * @brief Gets the sample rate (in Hz) for a Cin_DSP_MixerFormat
 */
CIN_DSP_GETTER(unsigned) Cin_DSP_MixerFormatGetSampleRate(
    const struct Cin_DSP_MixerFormat *format);

/**
 * @brief Sets the sample rate (in Hz) for a Cin_DSP_MixerFormat
 */
CIN_DSP_SETTER(void) Cin_DSP_MixerFormatSetSampleRate(
    struct Cin_DSP_MixerFormat *format,
    unsigned rate);

/**
 * @brief Gets the number of channels for a Cin_DSP_MixerFormat
 */
CIN_DSP_GETTER(unsigned) Cin_DSP_MixerFormatGetNumChannels(
    const struct Cin_DSP_MixerFormat *format);

/**
 * @brief Sets the sample format for a Cin_DSP_MixerFormat
 */
CIN_DSP_SETTER(void) Cin_DSP_MixerFormatSetNumChannels(
    struct Cin_DSP_MixerFormat *format,
    unsigned channels);

/**
 * @}
 */

/**
 * @brief Calculates the size of the required buffer for a conversion.
 *
 * Calculates how many bytes are needed to convert @p nbytes in the source
 * format specified by @p in_format to the destination format of @p out_format.
 *
 * @note @p nbytes must be evenly divisible by the size of the output type.
 *
 * @param nbytes Number of bytes to convert.
 * @param in_format Input format
 * @param out_format Output format
 * @return Buffer size to convert @p num_bytes from @p in_format to @p out_format
 */
CIN_DSP_CALL(unsigned) Cin_DSP_ConversionSize(unsigned num_bytes,
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
 *
 * @param in_bytes Number of bytes in in_data to consume
 * @param in_format Format of in_data to consume
 * @param in_data Input data to convert
 * @param out_format Format to convert to
 * @param out_data Destination buffer.
 * @return Number of bytes converted, or 0 on error.
 *
 * @sa Cin_DSP_ConversionSize
 */
CIN_DSP_CALL(unsigned) Cin_DSP_Convert(unsigned in_bytes,
    const struct Cin_DSP_MixerFormat *in_format,
    const void *in_data,
    const struct Cin_DSP_MixerFormat *out_format,
    void *out_data);


/**
 * @brief Mixes multiple audio buffers of the same format and size.
 *
 * Mixing can re-use an input buffer, although if out_data appears in the
 * in_data array it MUST be the first element and must only appear once.
 *
 * @param num_bytes Number of bytes of data to consume
 * @param format Format of data to consume
 * @param num_streams Number of streams in @p in_data
 * @param in_data Array of pointers to input data buffers
 * @param out_data Destination buffer. Must be at least @p num_bytes in size.
 * @return Number of bytes converted.
 */
CIN_DSP_CALL(unsigned) Cin_DSP_Mix(unsigned num_bytes,
    const struct Cin_DSP_MixerFormat *format,
    const unsigned num_streams,
    const void **in_data,
    void *out_data);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_DSP_H */
