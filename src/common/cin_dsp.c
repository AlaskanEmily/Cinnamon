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

#include "cin_dsp.h"

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

/*****************************************************************************/

static const double one_by_in8 = 0.007874015748031496,
    one_by_in16 = 3.051850948e-05,
    one_by_in32 = 4.656612875e-10;

/*****************************************************************************/

static const float one_by_in8f = 0.007874015748f,
    one_by_in16f = 3.051851e-05f,
    one_by_in32f = 4.656613e-10f;

/*****************************************************************************/

#define CIN_DSP_CORE(TYPE, MACRO) \
    do{ \
        register unsigned i; \
        for(i = 0; i + 4< num_bytes/sizeof(TYPE); i+=4) { \
            const TYPE s_0 = MACRO(i+0); \
            const TYPE s_1 = MACRO(i+1); \
            const TYPE s_2 = MACRO(i+2); \
            const TYPE s_3 = MACRO(i+3); \
            out[i+0] = s_0;\
            out[i+1] = s_1;\
            out[i+2] = s_2;\
            out[i+3] = s_3;\
        } \
        for( ; i < num_bytes/sizeof(TYPE); i++) { \
            out[i] = MACRO(i); \
        } \
    } while(0)

/*****************************************************************************/

#define CIN_DSP_CORE_MACRO_2(I) (data[0][(I)] + data[1][(I)])
#define CIN_DSP_CORE_MACRO_3(I) (CIN_DSP_CORE_MACRO_2(I) + data[2][(I)])
#define CIN_DSP_CORE_MACRO_4(I) (CIN_DSP_CORE_MACRO_3(I) + data[3][(I)])

/*****************************************************************************/

#define CIN_DSP_MIX(TYPE) \
void Cin_DSP_Mix ## TYPE(unsigned num_bytes, \
    unsigned num_streams, \
    const TYPE **in, \
    TYPE *out){ \
    \
    const TYPE *data[4]; \
    int n;\
    \
    switch(num_streams){ \
        case 0: return; \
        case 1: memmove(out, *in, num_bytes); return; \
        default: \
        case 4: data[3] = in[3]; \
        case 3: data[2] = in[2]; \
        case 2: data[0] = in[0]; data[1] = in[1]; \
    } \
    \
    switch(num_streams){ \
        case 2: CIN_DSP_CORE(TYPE, CIN_DSP_CORE_MACRO_2); return; \
        case 3: CIN_DSP_CORE(TYPE, CIN_DSP_CORE_MACRO_3); return; \
        case 4: CIN_DSP_CORE(TYPE, CIN_DSP_CORE_MACRO_4); return; \
        default: \
            n = num_streams - 3; \
            do{ \
                num_streams = 1;\
                data[0] = out; \
                data[num_streams++] = in[--n]; \
                if(n) \
                    data[num_streams++] = in[--n]; \
                if(n) \
                    data[num_streams++] = in[--n]; \
                Cin_DSP_Mix ## TYPE(num_bytes, num_streams, data, out); \
            }while(n != 0);\
    } \
}

/*****************************************************************************/

typedef signed char S8;
CIN_DSP_MIX(S8)

/*****************************************************************************/

typedef short S16;
CIN_DSP_MIX(S16)

/*****************************************************************************/

typedef int S32;
CIN_DSP_MIX(S32)

/*****************************************************************************/

typedef float Float32;
CIN_DSP_MIX(Float32)

/*****************************************************************************/

typedef double Float64;
CIN_DSP_MIX(Float64)

/*****************************************************************************/

static const unsigned cin_dsp_bytes_per_sample[] = {
    1, /**< CIN_DSP_FORMAT_S8       */
    2, /**< CIN_DSP_FORMAT_S16      */
    4, /**< CIN_DSP_FORMAT_S32      */
    4, /**< CIN_DSP_FORMAT_FLOAT32  */
    8, /**< CIN_DSP_FORMAT_FLOAT64  */
    1  /**< CIN_DSP_FORMAT_ULAW     */
};

/*****************************************************************************/

unsigned Cin_DSP_MixerFormatStructSize(){
    return sizeof(struct Cin_DSP_MixerFormat);
}

/*****************************************************************************/

void Cin_DSP_MixerFormatSet(struct Cin_DSP_MixerFormat *format,
    unsigned sample_format,
    unsigned sample_rate,
    unsigned num_channels){
    
    assert(sample_format < CIN_DSP_FORMAT_NUM_FORMATS);
    assert(format != NULL);
    
    format->sample_format = sample_format;
    format->sample_rate = sample_rate;
    format->num_channels = num_channels;
}

/*****************************************************************************/

unsigned Cin_DSP_MixerFormatGetFormat(
    const struct Cin_DSP_MixerFormat *format){
    return format->sample_format;
}

/*****************************************************************************/

void Cin_DSP_MixerFormatSetFormat(
    struct Cin_DSP_MixerFormat *format,
    unsigned sample_format){
    format->sample_format = sample_format;
}

/*****************************************************************************/

unsigned Cin_DSP_MixerFormatGetSampleRate(
    const struct Cin_DSP_MixerFormat *format){
    return format->sample_rate;
}

/*****************************************************************************/

void Cin_DSP_MixerFormatSetSampleRate(
    struct Cin_DSP_MixerFormat *format,
    unsigned rate){
    format->sample_rate = rate;
}

/*****************************************************************************/

unsigned Cin_DSP_MixerFormatGetNumChannels(
    const struct Cin_DSP_MixerFormat *format){
    return format->num_channels;
}

/*****************************************************************************/

void Cin_DSP_MixerFormatSetNumChannels(
    struct Cin_DSP_MixerFormat *format,
    unsigned channels){
    format->num_channels = channels;
}

/*****************************************************************************/

unsigned Cin_DSP_ConversionSize(unsigned num_bytes,
    const struct Cin_DSP_MixerFormat *in_format,
    const struct Cin_DSP_MixerFormat *out_format){
    
    assert(in_format->sample_format < CIN_DSP_FORMAT_NUM_FORMATS);
    assert(out_format->sample_format < CIN_DSP_FORMAT_NUM_FORMATS);
    
    const unsigned in_bytes_per_sample =
        cin_dsp_bytes_per_sample[in_format->sample_format];
    const unsigned out_bytes_per_sample =
        cin_dsp_bytes_per_sample[out_format->sample_format];
    
    const unsigned num_samples = num_bytes / in_bytes_per_sample;
    const unsigned num_frames = num_samples / in_format->num_channels;
    
    const unsigned out_samples = num_frames * out_format->num_channels;
    const unsigned out_bytes = out_samples * out_bytes_per_sample;
    
    unsigned in_rate = in_format->sample_rate;
    unsigned out_rate = out_format->sample_rate;
    
    assert(num_bytes % in_bytes_per_sample == 0);
    assert(num_samples % in_format->num_channels == 0);
    
    if(in_rate == out_rate){
        return out_bytes;
    }
    else{
        /* CHEAP HACK: Avoid overflow on very high sample rates */
        while(((in_rate & 1) | (out_rate & 1)) == 0){
            in_rate >>= 1;
            out_rate >>= 1;
        }
        
        return (out_bytes * out_rate) / in_rate;
    }
}

/*****************************************************************************/

#define CIN_DSP_CONVERT_SAMPLE(INTYPE, OUTTYPE, IN) \
    (( sizeof(INTYPE) == sizeof(OUTTYPE) ) ? \
        (IN) : \
    ( sizeof(INTYPE) < sizeof(OUTTYPE) ) ? \
        (IN << (sizeof(OUTTYPE) - sizeof(INTYPE))) : \
        (IN >> (sizeof(INTYPE) - sizeof(OUTTYPE))))\

/*****************************************************************************/

unsigned Cin_DSP_Convert(unsigned num_bytes,
    const struct Cin_DSP_MixerFormat *in_format,
    const void *in_data,
    const struct Cin_DSP_MixerFormat *out_format,
    void *out_data){
    
    const ldiv_t rate = ldiv(out_format->sample_rate, in_format->sample_rate);
    
    /* HUGE HUGE HACK! */
    const unsigned n = Cin_DSP_ConversionSize(num_bytes, in_format, out_format);
    
    if(n < num_bytes){
        memcpy(out_data, in_data, n);
    }
    else{
        memcpy(out_data, in_data, num_bytes);
        memset(((unsigned char)out_data) + num_bytes, 0, n - num_bytes);
    }
    return n;
}

/*****************************************************************************/

unsigned Cin_DSP_Mix(unsigned num_bytes,
    const struct Cin_DSP_MixerFormat *format,
    const unsigned num_streams,
    const void **in_data,
    void *out_data){
    
    switch(format->sample_format){
        case CIN_DSP_FORMAT_S8:
            Cin_DSP_MixS8(num_bytes, num_streams, in_data, out_data); break;
            
        case CIN_DSP_FORMAT_S16:
            Cin_DSP_MixS16(num_bytes, num_streams, in_data, out_data); break;
            
        case CIN_DSP_FORMAT_S32:
            Cin_DSP_MixS32(num_bytes, num_streams, in_data, out_data); break;
            
        case CIN_DSP_FORMAT_FLOAT32:
            Cin_DSP_MixFloat32(num_bytes, num_streams, in_data, out_data); break;
            
        case CIN_DSP_FORMAT_FLOAT64:
            Cin_DSP_MixFloat64(num_bytes, num_streams, in_data, out_data); break;
        
        default:
            return 0;
    }
    return num_bytes;
}
