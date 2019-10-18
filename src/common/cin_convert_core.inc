
/*****************************************************************************/
/* This file generates the converter core when included.
 * It expects IN_TYPE and OUT_TYPE to be defined as types. If COMMON_TYPE is
 * defined, then it is used as the intermediate type. Otherwise, we use the
 * output type as the intermediate type.
 * If CIN_DSP_CONVERT_SAMPLE_IN_TO_COMMON is defined, then it is used as a
 * macro to convert from the input to the common type.
 * If CIN_DSP_CONVERT_SAMPLE_TO_OUT is defined, then it is used as a macro to
 * convert from the common to the output type.
 * This will undefine IN_TYPE, OUT_TYPE, COMMON_TYPE, 
 * CIN_DSP_CONVERT_SAMPLE_IN_TO_COMMON and
 * CIN_DSP_CONVERT_SAMPLE_COMMON_TO_OUT.
 * This is meant just to make it more concise to use. */
/*****************************************************************************/

#ifndef COMMON_TYPE
#define COMMON_TYPE OUT_TYPE
#define CIN_DSP_CONVERT_SAMPLE_COMMON_TO_OUT(X) (X)
#endif

#ifndef CIN_DSP_CONVERT_SAMPLE_IN_TO_COMMON
#define CIN_DSP_CONVERT_SAMPLE_IN_TO_COMMON(X) \
    CIN_DSP_CONVERT_SAMPLE_INT_TO_INT(IN_TYPE, COMMON_TYPE, X)
#endif

#ifndef CIN_DSP_CONVERT_SAMPLE_COMMON_TO_OUT
#define CIN_DSP_CONVERT_SAMPLE_COMMON_TO_OUT(X)\
    CIN_DSP_CONVERT_SAMPLE_INT_TO_INT(COMMON_TYPE, OUT_TYPE, X)
#endif

/* Used in the stereo to mono specialization. */
#define CIN_DSP_STEREO_TO_MONO(IN_PTR, COMMON_VALUE, DEST_PTR) do{ \
        const IN_TYPE *const STM_in_ptr = (IN_PTR); \
        const IN_TYPE STM_in_sample0 = in[0]; \
        const IN_TYPE STM_in_sample1 = in[1]; \
        const COMMON_TYPE STM_common_sample0 = \
            CIN_DSP_CONVERT_SAMPLE_IN_TO_COMMON(STM_in_sample0); \
        const COMMON_TYPE STM_common_sample1 = \
            CIN_DSP_CONVERT_SAMPLE_IN_TO_COMMON(STM_in_sample1); \
        const COMMON_TYPE STM_common_value = (COMMON_VALUE) = \
            (STM_common_sample0 / (COMMON_TYPE)2) + \
            (STM_common_sample1 / (COMMON_TYPE)2); \
        (void)sizeof((COMMON_VALUE)); \
        (DEST_PTR)[0] = \
            CIN_DSP_CONVERT_SAMPLE_COMMON_TO_OUT(STM_common_value); \
    } while(0)
    

static unsigned CIN_DSP_CONVERT_NAME (
    const unsigned num_in_frames,
    const IN_TYPE *in,
    OUT_TYPE *out,
    const unsigned in_sample_rate,
    const unsigned in_num_channels,
    const unsigned out_sample_rate,
    const unsigned out_num_channels){
    
    const unsigned num_common_channels =
        (out_num_channels > in_num_channels) ?
        in_num_channels : out_num_channels;
    
    const ldiv_t rate = ldiv(out_sample_rate, in_sample_rate);
    int at = 0;
    unsigned i, channel;
    /* Used for the return value */
    const unsigned char *const original_out = (unsigned char*)out;
    
    unsigned out_frames = 0;
    
    for(i = 0; i < num_in_frames; i++){
        
        out_frames = 0;
        at -= rate.rem;
        if(at < 0){
            at += in_sample_rate;
            out_frames++;
        }
        
        /* Check if there are any samples to output.
         * This can be false when down-sampling.
         */
        out_frames += rate.quot;
        if(out_frames != 0){
            
            /* Special case for stereo -> mono. Ideally this would NOT be
             * checked inside the loop :(
             */
            if(in_num_channels == 2 && out_num_channels == 1){
                COMMON_TYPE unused;
                CIN_DSP_STEREO_TO_MONO(in, unused, out);
            }
            else{
                /* Hold on to the final channel we processed, so that we can
                 * paste it over all the remaining channels.
                 */
                OUT_TYPE last_out;
                /* Output the first frame verbatim. This is an optimization for
                 * downsampling, and also uses a slightly cheaper calculation for
                 * the first output frame in other cases.
                 */
                for(channel = 0; channel < num_common_channels; channel++){
                    const IN_TYPE in_sample = in[channel];
                    const COMMON_TYPE common_sample =
                        CIN_DSP_CONVERT_SAMPLE_IN_TO_COMMON(in_sample);
                    out[channel] =
                        last_out =
                        CIN_DSP_CONVERT_SAMPLE_COMMON_TO_OUT(common_sample);
                }
                
                /* Put the last channel we processed over any remaining
                 * channels.
                 */
                for(;channel < out_num_channels; channel++){
                    out[channel] = last_out;
                }
            }
            
            out += out_num_channels;
            in += in_num_channels;
            
            out_frames--;

            if(out_frames != 0 && (i + 1) < num_in_frames){
                COMMON_TYPE common_step, common_sample_begin;
                
                /* Special case for stereo -> mono. Ideally this would NOT be
                 * checked inside the loop :(
                 */
                if(in_num_channels == 2 && out_num_channels == 1){
                    unsigned frame = 0;
                    /* Get the input samples we will lerp between. */
                    COMMON_TYPE common_sample, common_sample_end;
                    OUT_TYPE unused;
                    CIN_DSP_STEREO_TO_MONO(in+0, common_sample, &unused);
                    CIN_DSP_STEREO_TO_MONO(in+2, common_sample_end, &unused);
                    /* This is slightly lossy with integer common types and
                     * small steps. */
                    common_step =
                        (common_sample_end - common_sample) /
                        ((COMMON_TYPE)out_frames);
                    
                    while(1){
                        out[0] = common_sample;
                        if(++frame == out_frames)
                            break;
                        common_sample += common_step;
                        out++;
                    }
                    
                    /* This skips the below loops in a way that plays well with
                     * GCC, clang, and MSVC's optimizers.
                     */
                    channel = out_num_channels;
                }
                else{
                    channel = 0;
                }
                
                /* Process per-channel, since we need to do lerps. */
                for(;channel < num_common_channels; channel++){
                    /* Get the input samples we will lerp between. */
                    const IN_TYPE in_sample_start = in[channel];
                    const IN_TYPE in_sample_end =
                        in[channel + in_num_channels];
                    
                    /* Do a simple lerp between the start and the end in the
                     * common type. Ideally we would be using the derivative of
                     * the waveform to lerp smartly, but linear approximation
                     * works OK and is very simple to implement */
                    COMMON_TYPE common_sample =
                        common_sample_begin =
                        CIN_DSP_CONVERT_SAMPLE_IN_TO_COMMON(in_sample_start);
                    const COMMON_TYPE common_sample_end =
                        CIN_DSP_CONVERT_SAMPLE_IN_TO_COMMON(in_sample_end);
                    
                    /* Will be advanced on each frame */
                    OUT_TYPE *dest = out + channel;
                    unsigned frame = 0;
                    
                    /* This is slightly lossy with integer common types and
                     * small steps. */
                    common_step =
                        (common_sample_end - common_sample) /
                        ((COMMON_TYPE)out_frames);
                    
                    while(1){
                        *dest = common_sample;
                        if(++frame == out_frames)
                            break;
                        common_sample += common_step;
                        dest += out_num_channels;
                    }
                }
                
                /* Copy the last written channel over all remaining channels.
                 */
                for(; channel < out_num_channels; channel++){
                    /* Will be advanced on each frame */
                    OUT_TYPE *dest = out + channel;
                    COMMON_TYPE common_sample = common_sample_begin;
                    unsigned frame = 0;
                    while(1){
                        *dest = common_sample;
                        if(++frame == out_frames)
                            break;
                        common_sample += common_step;
                        dest += out_num_channels;
                    }
                }
                
                in += in_num_channels;
                out += out_num_channels * out_frames;
            }
        }
    }
    
    return ((unsigned char*)out) - original_out;
}

#undef CIN_DSP_STEREO_TO_MONO
#undef COMMON_TYPE
#undef OUT_TYPE
#undef IN_TYPE
#undef CIN_DSP_CONVERT_SAMPLE_IN_TO_COMMON
#undef CIN_DSP_CONVERT_SAMPLE_COMMON_TO_OUT
#undef CIN_DSP_CONVERT_NAME

/*****************************************************************************/
