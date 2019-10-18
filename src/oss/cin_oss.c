/* Copyright (c) 2019 Alaskan Emily, Transnat Games
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cinnamon.h"
#include "cin_oss.h"
#include "cin_oss_driver.h"
#include "cin_mixer_sound.h"
#include "cin_dsp.h"

#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#if defined __GNUC__
#include <alloca.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/*****************************************************************************/

void *Cin_OSS_ThreadFunc(void *v){
    struct Cin_MixerSound *channels[CIN_OSS_SOUND_CHANNELS];
    unsigned active_channels = 0, i;
    struct Cin_Driver *const drv = v;
    struct timespec tm;

    const Cin_MixerSound_PtrPtr drv_channels = Cin_OSS_GetChannels(drv);
    const unsigned rate = Cin_OSS_GetSampleRate(drv);
    const int dev = Cin_OSS_GetDevice(drv);
    const int num_channels = Cin_OSS_GetNumChannels(drv);
    const int format = Cin_OSS_GetFormat(drv);
    const int bytes_per_frame = CIN_FORMAT_BYTES_PER_SAMPLE(format) * num_channels;
    const int buffer_size = (bytes_per_frame * rate) / 10;
    
    const void *input_data[CIN_OSS_SOUND_CHANNELS+1];
    
    void *const buffer = malloc(buffer_size);
    
    CIN_DSP_NEW_STACK_FMT_STRUCT(dsp_fmt);
    
    CIN_DSP_MIXER_FORMAT_SET(dsp_fmt,
        format,
        rate,
        num_channels);
    
iter:
    
    clock_gettime(CLOCK_REALTIME, &tm);
    
    if(Cin_OSS_ProcessCommands(drv) == 1){
        free(buffer);
        CIN_DSP_FREE_STACK_FMT_STRUCT(dsp_fmt);
        return NULL;
    }
    
    active_channels = 0;
    for(i = 0; i < CIN_OSS_SOUND_CHANNELS; i++){
        struct Cin_MixerSound *const snd = drv_channels[i];
        if(snd != NULL){
            channels[active_channels++] = snd;
            
            /* Clear the sound if this will be its final play. */
            if(snd->position + buffer_size >= snd->byte_len){
                drv_channels[i] = NULL;
            }
        }
    }
    
    if(active_channels != 0){
        /* First, mix any finalizing channels (channels without enough data to
         * fill the buffer)
         */
        i = 0;
        
        /* Find the first channel which is being finalized, if any. */
        do{
            if(channels[i]->position + buffer_size >
                channels[i]->byte_len){
                break;
            }
        }while(++i < active_channels);
        
        /* Check this only if we broke early (found a finalizing channel) */
        if(i != active_channels){
            input_data[0] = buffer;
            
            /* Very fortunately, zero-init is ALSO zero in float and double */
            memset(buffer, 0, buffer_size);
            
            do{
                struct Cin_MixerSound *const snd = channels[i];
                const int remaining_length =
                    snd->byte_len - snd->position;
                if(remaining_length < buffer_size){
                    input_data[1] = CIN_MIXER_SOUND_PCM(channels[i]) +
                        snd->position;
                    
                    Cin_DSP_Mix(remaining_length,
                        dsp_fmt,
                        2,
                        input_data,
                        buffer);
                    
                    /* Pull out this channel by simply swapping in the final
                     * element. This makes removing any element constant time,
                     * and is acceptable because the order of the channels is
                     * not important.
                     */
                    channels[i] = channels[--active_channels];
                }
                else{
                    i++;
                }
            }while(i < active_channels);
            
            /* Set i = 1, so that we do not place anything over the input_data
             * element containing our mixed finalizing sounds.
             */
            i = 1;
        }
        else{
            /* No finalizing sounds, so no extra inputs. */
            i = 0;
        }
        
        /* No need to remix if all channels were finalizing. */
        if(active_channels != 0){
            register unsigned source_i = 0;
            do{
                input_data[i++] = CIN_MIXER_SOUND_PCM(channels[source_i]) +
                    channels[source_i]->position;
                channels[source_i]->position += buffer_size;
            }while(++source_i < active_channels);
        }
        /* i contains the final element placed into input_data, regardless
         * of how many were from the input channels.
         */
        Cin_DSP_Mix(buffer_size, dsp_fmt, i, input_data, buffer);
    }
    else{
        memset(buffer, 0, buffer_size);
    }
    
    write(dev, buffer, buffer_size);
    
    /* Sleep for 1/10th of a second after the last call to ProcessCommands. */
    {
        struct timespec now;
        long ms_diff = 0;
        clock_gettime(CLOCK_REALTIME, &now);
        if(now.tv_sec != tm.tv_sec)
            ms_diff = 1000;
        ms_diff += (now.tv_nsec - tm.tv_nsec) / 1000000;
        
        if(ms_diff < 0)
            ms_diff = 0;
        else if(ms_diff > 99)
            ms_diff = 99;
        
#ifdef __linux__
        now.tv_sec = 0;
        now.tv_nsec = (100 - ms_diff) * 1000000;
        nanosleep(&now, NULL);
#else
        usleep(ms_diff * 1000);
#endif
    }
    
    goto iter;
}
