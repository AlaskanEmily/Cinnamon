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

#include "cin_dsp.h"

// #include "cin_common.h"

#include <algorithm>
#include <iterator>
#include <limits>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

static const unsigned cin_dsp_bytes_per_sample[] = {
    1, ///< CIN_DSP_FORMAT_S8
    2, ///< CIN_DSP_FORMAT_S16
    4, ///< CIN_DSP_FORMAT_S32
    4, ///< CIN_DSP_FORMAT_FLOAT32
    8, ///< CIN_DSP_FORMAT_FLOAT64
    1 ///< CIN_DSP_FORMAT_ULAW
};

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
        // CHEAP HACK: Avoid overflow on very high sample rates
        while(((in_rate & 1) | (out_rate & 1)) == 0){
            in_rate >>= 1;
            out_rate >>= 1;
        }
        
        return (out_bytes * out_rate) / in_rate;
    }
}

/*****************************************************************************/

namespace Cin {

static const float one_by_in8 = 0.007874015748031496f,
    one_by_in16 = 3.051850948e-05f,
    one_by_in32 = 4.656612875e-10f;

static inline void convert_sample(uint8_t in, uint8_t &out){
    out = in;
}

static inline void convert_sample(uint8_t in, int16_t &out){
    int16_t a = in;
    a -= 126;
    out = a << 17;
}

template<typename OutputT>
static inline void convert_sample(uint8_t in, OutputT &out){
    out = in;
}

/*****************************************************************************/

static inline void convert_sample(int8_t in, int8_t &out){ out = in; }
static inline void convert_sample(int8_t in, int16_t &out){ out = in << 8; }
static inline void convert_sample(int8_t in, int32_t &out){ out = in << 24; }
static inline void convert_sample(int8_t in, float &out){
    out = static_cast<float>(in) * one_by_in8;
}
static inline void convert_sample(int8_t in, double &out){
    out = static_cast<double>(in) * static_cast<double>(one_by_in8);
}

/*****************************************************************************/

static inline void convert_sample(int16_t in, int8_t &out){ out = in >> 8; }
static inline void convert_sample(int16_t in, int16_t &out){ out = in; }
static inline void convert_sample(int16_t in, int32_t &out){ out = in << 16; }
static inline void convert_sample(int16_t in, float &out){
    out = static_cast<float>(in) * one_by_in16;
}
static inline void convert_sample(int16_t in, double &out){
    out = static_cast<double>(in) * static_cast<double>(one_by_in16);
}

/*****************************************************************************/

static inline void convert_sample(int32_t in, int8_t &out){ out = static_cast<int32_t>(in) >> 24; }
static inline void convert_sample(int32_t in, int16_t &out){ out = in >> 16; }
static inline void convert_sample(int32_t in, int32_t &out){ out = in; }
static inline void convert_sample(int32_t in, float &out){
    out = static_cast<float>(in) * one_by_in32;
}
static inline void convert_sample(int32_t in, double &out){
    out = static_cast<double>(in) * static_cast<double>(one_by_in32);
}

/*****************************************************************************/

static inline void convert_sample(const float &in, int8_t &out){
    out = in * static_cast<float>(INT8_MAX-1);
}
static inline void convert_sample(const float &in, int16_t &out){
    out = in * static_cast<float>(INT16_MAX-1);
}
static inline void convert_sample(const float &in, int32_t &out){
    out = in * static_cast<float>(INT32_MAX-1);
}
static inline void convert_sample(const float &in, float &out){
    out = in;
}
static inline void convert_sample(const float &in, double &out){
    out = in;
}
static inline void convert_sample(const double &in, int8_t &out){
    convert_sample(static_cast<float>(in), out);
}
static inline void convert_sample(const double &in, int16_t &out){
    convert_sample(static_cast<float>(in), out);
}
static inline void convert_sample(const double &in, int32_t &out){
    convert_sample(static_cast<float>(in), out);
}
static inline void convert_sample(const double &in, float &out){
    out = static_cast<float>(in);
}
static inline void convert_sample(const double &in, double &out){
    out = in;
}

/*****************************************************************************/

template<typename SampleT>
class SampleIterator {
    SampleT *m_data;
    const unsigned char m_num_channels;
public:
    
    typedef SampleT *pointer;
    typedef SampleT &reference;
    typedef SampleT value_type;
    typedef unsigned difference_type;
    typedef std::random_access_iterator_tag iterator_category;
    
    SampleIterator(SampleT *data, unsigned char num_channels)
      : m_data(data)
      , m_num_channels(num_channels){
        
    }
    
    SampleIterator(void *data, unsigned char num_channels)
      : m_data(static_cast<SampleT*>(data))
      , m_num_channels(num_channels){
        
    }
    
    inline bool operator==(const SampleIterator<SampleT> &other){
        assert((m_data == other.m_dat) ?
            (other.m_num_channels == m_num_channels) :
            true);
        return m_data == other.m_data;
    }
    
    inline SampleIterator<SampleT> &operator=(SampleIterator<SampleT> &other){
        m_data = other.m_data;
        m_num_channels == other.m_num_channels;
    }
    
    inline SampleIterator<SampleT> &operator++(){
        m_data += m_num_channels;
        return *this;
    }
    
    inline SampleIterator<SampleT> operator++(int i){
        (void)i;
        SampleT *const data = m_data;
        m_data = data + m_num_channels;
        return SampleIterator<SampleT>(data, m_num_channels);
    }

    inline SampleIterator<SampleT> &operator+=(int count){
        m_data += m_num_channels * count;
        return *this;
    }

    inline SampleIterator<SampleT> operator+(int count){
        SampleT *const data = m_data + (m_num_channels * count);
        return SampleIterator<SampleT>(data, m_num_channels);
    }
    
    inline SampleIterator<SampleT> &operator--(){
        m_data -= m_num_channels;
        return *this;
    }
    
    inline SampleIterator<SampleT> operator--(int i){
        (void)i;
        SampleT *const data = m_data;
        m_data = data - m_num_channels;
        return SampleIterator<SampleT>(data, m_num_channels);
    }

    inline SampleIterator<SampleT> &operator-=(int count){
        m_data -= m_num_channels * count;
        return *this;
    }

    inline SampleIterator<SampleT> operator-(int count){
        SampleT *const data = m_data - (m_num_channels * count);
        return SampleIterator<SampleT>( data, m_num_channels);
    }
    
    inline unsigned operator-(const SampleIterator<SampleT> &other){
        return m_data - other.m_data;
    }
    
    inline unsigned numChannels() const {
        return m_num_channels;
    }
    
    inline SampleT channel(unsigned i) const {
        assert(i < m_num_channels);
        return m_data[i];
    }
    
    inline SampleT &channel(unsigned i) {
        assert(i < m_num_channels);
        return m_data[i];
    }
};

/*****************************************************************************/

template<typename T>
static inline T mix(T a, T b){
    const int32_t product = static_cast<int32_t>(a) * static_cast<int32_t>(b);
    const int32_t sum = static_cast<int32_t>(a) + static_cast<int32_t>(b);
    if(a < 0 && b < 0){
        return sum - (product / std::numeric_limits<T>::min());
    }
    else if(a > 0 && b > 0){
        return sum - (product / std::numeric_limits<T>::max());
    }
    else{
        return sum;
    }
}

/*****************************************************************************/

template<typename T>
static inline float mix_float(T a, T b){
    const T sum = a + b;
    const T product = a * b;
    if(a < static_cast<T>(0) && b < static_cast<T>(0)){
        return sum + product;
    }
    else if(a > static_cast<T>(0) && b > static_cast<T>(0)){
        return sum - product;
    }
    else{
        return sum;
    }
}

/*****************************************************************************/

template<>
inline int32_t mix<int32_t>(int32_t a, int32_t b){
    // Special case for 32 bit ints to use larger intermediates.
    const int64_t product = static_cast<int64_t>(a) * static_cast<int64_t>(b);
    const int64_t sum = static_cast<int64_t>(a) + static_cast<int64_t>(b);
    if(a < 0 && b < 0){
        return sum - (product / INT32_MIN);
    }
    else if(a > 0 && b > 0){
        return sum - (product / INT32_MAX);
    }
    else{
        return sum;
    }
}

/*****************************************************************************/

template<>
inline float mix<float>(float a, float b){
    return mix_float<float>(a, b);
}

/*****************************************************************************/

template<>
inline double mix<double>(double a, double b){
    return mix_float<double>(a, b);
}

/*****************************************************************************/

template<typename InputT, typename OutputT>
static inline unsigned DSP(const unsigned num_input_frames,
    SampleIterator<const InputT> in,
    const unsigned in_sample_rate,
    SampleIterator<OutputT> out,
    const unsigned out_sample_rate){
    
    
    const SampleIterator<OutputT> start_out = out;
    
    const ldiv_t rate = ldiv(out_sample_rate, in_sample_rate);
    
    const unsigned in_channels = in.numChannels();
    const unsigned out_channels = out.numChannels();
    int at = 0;
    for(unsigned i = 0; i < num_input_frames; i++){
        unsigned output_samples = 0;
        at -= rate.rem;
        if(at < 0){
            at += in_sample_rate;
            output_samples++;
        }
        
        // Check if there are any samples to output.
        // This can be false when down-sampling.
        if((output_samples += rate.quot) > 0){
            
            const SampleIterator<const InputT> cur_sample = in;
            const SampleIterator<const InputT> &next_sample = ++in;
            
            // Output the first frame verbatim. This is an optimization for
            // downsampling, and avoids any extra calculations for the first
            // output frame.
            {
                unsigned channel = 0;
                while(channel < in_channels){
                    out.channel(channel) = cur_sample.channel(channel);
                    channel++;
                }
                
                const unsigned last_channel = channel - 1;
                while(channel < out_channels){
                    out.channel(channel) = cur_sample.channel(last_channel);
                    channel++;
                }
                
                ++out;
            }
            
            if(output_samples > 1){
                unsigned channel = 0;
                OutputT start, step;
                while(channel < in_channels){
                    OutputT end;
                    convert_sample(cur_sample.channel(channel), start);
                    convert_sample(next_sample.channel(channel), end);
                    step = start - end;
                    
                    OutputT value = start;
                    unsigned frame = 1;
                    do{
                        value += step;
                        (out + frame).channel(channel) = value;
                    }while(++frame < output_samples);
                    channel++;
                    out += frame;
                }
                
                // Dupe the final input channel for all remaining output channels
                while(channel < out_channels){
                    OutputT value = start;
                    unsigned frame = 1;
                    do{
                        value += step;
                        (out + frame).channel(channel) = value;
                    }while(++frame < output_samples);
                    channel++;
                    out += frame;
                }
            }
        }
        
        ++in;
    }
    
    return out.numChannels() * sizeof(OutputT) * std::distance(start_out, out);
}

/*****************************************************************************/

} // namespace Cin

/*****************************************************************************/

unsigned Cin_DSP_Convert(unsigned in_bytes,
    const struct Cin_DSP_MixerFormat *in_format,
    const void *in_data,
    const struct Cin_DSP_MixerFormat *out_format,
    void *out_data){
    
    using Cin::SampleIterator;
    
    assert(in_format->sample_format < CIN_DSP_FORMAT_NUM_FORMATS);
    assert(out_format->sample_format < CIN_DSP_FORMAT_NUM_FORMATS);
    
    const unsigned in_bytes_per_sample =
        cin_dsp_bytes_per_sample[in_format->sample_format];
    const unsigned num_samples = in_bytes / in_bytes_per_sample;
    const unsigned num_frames = num_samples / in_format->num_channels;
    
#define OUTPUT_CASE(IN_TYPE)\
    do{\
        SampleIterator<const IN_TYPE>\
            in_iter((const IN_TYPE*)in_data, in_format->num_channels);\
        switch(out_format->sample_format){\
            case CIN_DSP_FORMAT_S8:\
                return Cin::DSP(num_frames, in_iter, in_format->sample_rate,\
                    SampleIterator<int8_t>((uint8_t*)out_data, out_format->num_channels),\
                    out_format->sample_rate);\
            case CIN_DSP_FORMAT_S16:\
                return Cin::DSP(num_frames, in_iter, in_format->sample_rate,\
                    SampleIterator<int16_t>((int16_t*)out_data, out_format->num_channels),\
                    out_format->sample_rate);\
            case CIN_DSP_FORMAT_S32:\
                return Cin::DSP(num_frames, in_iter, in_format->sample_rate,\
                    SampleIterator<int32_t>((int32_t*)out_data, out_format->num_channels),\
                    out_format->sample_rate);\
            case CIN_DSP_FORMAT_FLOAT32:\
                return Cin::DSP(num_frames, in_iter, in_format->sample_rate,\
                    SampleIterator<float>((float*)out_data, out_format->num_channels),\
                    out_format->sample_rate);\
            case CIN_DSP_FORMAT_FLOAT64:\
                return Cin::DSP(num_frames, in_iter, in_format->sample_rate,\
                    SampleIterator<double>((double*)out_data, out_format->num_channels),\
                    out_format->sample_rate);\
            default: return 0;\
        }\
    }while(1);

    
    switch(in_format->sample_format){
        case CIN_DSP_FORMAT_S8:
            OUTPUT_CASE(int8_t);
        case CIN_DSP_FORMAT_S16:
            OUTPUT_CASE(int16_t);
        case CIN_DSP_FORMAT_S32:
            OUTPUT_CASE(int32_t);
        case CIN_DSP_FORMAT_FLOAT32:
            OUTPUT_CASE(float);
        case CIN_DSP_FORMAT_FLOAT64:
            OUTPUT_CASE(double);
        default: return 0;
    }
}

unsigned Cin_DSP_Mix(unsigned num_bytes,
    const struct Cin_DSP_MixerFormat *format,
    const void *in_data0,
    const void *in_data1,
    void *out_data){

#define MIX_CASE(TYPE)\
    do{\
        const TYPE *const in0 = (const TYPE*)in_data0;\
        const TYPE *const in1 = (const TYPE*)in_data1;\
        TYPE *const out = (TYPE*)out_data;\
        std::transform(in0, in0 + (num_bytes / sizeof(TYPE)), in1, out, Cin::mix<TYPE>);\
        return num_bytes;\
    }while(1)
    
    switch(format->sample_format){
        case CIN_DSP_FORMAT_S8:
            MIX_CASE(int8_t);
        case CIN_DSP_FORMAT_S16:
            MIX_CASE(int16_t);
        case CIN_DSP_FORMAT_S32:
            MIX_CASE(int32_t);
        case CIN_DSP_FORMAT_FLOAT32:
            MIX_CASE(float);
        case CIN_DSP_FORMAT_FLOAT64:
            MIX_CASE(double);
        default: return 0;
    }
}
