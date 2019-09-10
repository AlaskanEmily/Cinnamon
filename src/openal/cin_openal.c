/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cinnamon.h"
#include "cin_openal.h"

#include <stddef.h>
#include <assert.h>

/*****************************************************************************/

CIN_PRIVATE_PURE(ALuint)
    Cin_CinFormatToOpenALFormat(enum Cin_Format f, unsigned num_channels){
    
    switch(f){
        case Cin_eFormatS8:
            if(num_channels == 1)
                return AL_FORMAT_MONO8;
            if(num_channels == 2)
                return AL_FORMAT_STEREO8;
            if(num_channels == 4)
                return AL_FORMAT_QUAD8;
            assert(0);
            return 0;
        case Cin_eFormatS16:
            if(num_channels == 1)
                return AL_FORMAT_MONO16;
            if(num_channels == 2)
                return AL_FORMAT_STEREO16;
            if(num_channels == 4)
                return AL_FORMAT_QUAD16;
            assert(0);
            return 0;
        case Cin_eFormatS32:
            assert(0);
            return 0;
        case Cin_eFormatFloat32:
            if(num_channels == 1)
                return AL_FORMAT_MONO_FLOAT32;
            if(num_channels == 2)
                return AL_FORMAT_STEREO_FLOAT32;
            assert(0);
            return 0;
        case Cin_eFormatFloat64:
            if(num_channels == 1)
                return AL_FORMAT_MONO_DOUBLE_EXT;
            if(num_channels == 2)
                return AL_FORMAT_STEREO_DOUBLE_EXT;
            assert(0);
            return 0;
        case Cin_eFormatULaw8:
            if(num_channels == 1)
                return AL_FORMAT_MONO_MULAW_EXT;
            if(num_channels == 2)
                return AL_FORMAT_STEREO_MULAW_EXT;
            if(num_channels == 4)
                return AL_FORMAT_QUAD_MULAW;
            assert(0);
            return 0;
    }
    assert(0);
    return 0;
}

/*****************************************************************************/

CIN_PRIVATE(int) Cin_CinExtension(const struct Cin_Driver *drv, int ext){
    if((drv->ext & 1) == 0){
        short s = 0;
        alcMakeContextCurrent(drv->ctx);
        if(alIsExtensionPresent("AL_EXT_float32"))
            s |= (1 << CIN_FLOAT_SUPPORTED);
        
        if(alIsExtensionPresent("AL_EXT_double"))
            s |= (1 << CIN_DOUBLE_SUPPORTED);
        
        if(alIsExtensionPresent("AL_EXT_MULAW") ||
            alIsExtensionPresent("AL_EXT_mulaw") ||
            alIsExtensionPresent("AL_EXT_ULAW") ||
            alIsExtensionPresent("AL_EXT_ulaw") )
            s |= (1 << CIN_MULAW_SUPPORTED);
        
        if(alIsExtensionPresent("AL_EXT_MCFORMATS") ||
            alIsExtensionPresent("AL_EXT_mcformats"))
            s |= (1 << CIN_QUAD_SUPPORTED);
        
        if(alIsExtensionPresent("AL_EXT_MULAW_MCFORMATS") ||
            alIsExtensionPresent("AL_EXT_mulaw_mcformats") ||
            alIsExtensionPresent("AL_EXT_ULAW_MCFORMATS") ||
            alIsExtensionPresent("AL_EXT_ulaw_mcformats"))
            s |= (1 << CIN_MULAW_QUAD_SUPPORTED);
        
        ((struct Cin_Driver *)drv)->ext = s;
        
        return s & (1 << ext);
    }
    
    return drv->ext & (1 << ext);
}

/*****************************************************************************/

CIN_PRIVATE(int) Cin_CleanOpenALSound(ALuint snd, ALuint *out){
    ALint i;
    alGetSourcei(snd, AL_BUFFERS_PROCESSED, &i);
    if(i > 0){
        ALuint buffers[16];
        if(out != NULL){
            --i;
            alSourceUnqueueBuffers(snd, 1, out);
        }
        do{
            const unsigned to_delete = (i >= 16) ? 16 : i;
            alSourceUnqueueBuffers(snd, to_delete, buffers);
            alDeleteBuffers(to_delete, buffers);
            i -= to_delete;
        }while(i > 0);
        return 1;
    } /* if(i > 0) */
    return 0;
}

/*****************************************************************************/

unsigned Cin_StructDriverSize(){
    return sizeof(struct Cin_Driver);
}

/*****************************************************************************/

enum Cin_DriverError Cin_CreateDriver(struct Cin_Driver *drv){
    ALCcontext *ctx;
    ALCdevice *dev;
    
    assert(drv != NULL);
    
    dev = alcOpenDevice(NULL);
    if(dev == NULL){
        return Cin_eDriverNoDevice;
    }
    
    ctx = alcCreateContext(dev, NULL);
    
    if(ctx == NULL){
        alcCloseDevice(dev);
        return Cin_eDriverFailure;
    }
    
    drv->ctx = ctx;
    drv->dev = dev;
    drv->ext = 0;
    
    return Cin_eDriverSuccess;
}

/*****************************************************************************/

void Cin_DestroyDriver(struct Cin_Driver *drv){
    assert(drv != NULL);
    assert(drv->ctx != NULL);
    assert(drv->dev != NULL);
    
    alcDestroyContext(drv->ctx);
    alcCloseDevice(drv->dev);
}

/*****************************************************************************/

enum Cin_DriverError Cin_DriverSupportsFormat(
    const struct Cin_Driver *drv,
    enum Cin_Format format,
    unsigned num_channels){
    
    assert(drv != NULL);
    assert(drv->ctx != NULL);
    assert(drv->dev != NULL);
    
    switch(format){
        case Cin_eFormatS16: /* FALLTHROUGH */
        case Cin_eFormatS8:
            if(num_channels == 1 || num_channels == 2){
                return Cin_eDriverSuccess;
            }
            else if(num_channels == 4){
                return Cin_CinExtension(drv, CIN_QUAD_SUPPORTED) ?
                    Cin_eDriverSuccess : Cin_eDriverUnsupportedNumChannels;
            }
            else{
                return Cin_eDriverUnsupportedNumChannels;
            }
        case Cin_eFormatS32:
            return Cin_eDriverUnsupportedFormat;
        case Cin_eFormatFloat32:
            if(num_channels == 1 || num_channels == 2){
                return Cin_CinExtension(drv, CIN_FLOAT_SUPPORTED) ?
                    Cin_eDriverSuccess : Cin_eDriverUnsupportedFormat;
            }
            else{
                return Cin_eDriverUnsupportedNumChannels;
            }
        case Cin_eFormatFloat64:
            if(num_channels == 1 || num_channels == 2){
                return Cin_CinExtension(drv, CIN_DOUBLE_SUPPORTED) ?
                    Cin_eDriverSuccess : Cin_eDriverUnsupportedFormat;
            }
            else{
                return Cin_eDriverUnsupportedNumChannels;
            }
        case Cin_eFormatULaw8:
            if(!Cin_CinExtension(drv, CIN_MULAW_SUPPORTED))
                return Cin_eDriverUnsupportedFormat;
            if(num_channels == 1 || num_channels == 2){
                return Cin_eDriverSuccess;
            }
            else if(num_channels == 4){
                return Cin_CinExtension(drv, CIN_MULAW_QUAD_SUPPORTED) ?
                    Cin_eDriverSuccess : Cin_eDriverUnsupportedNumChannels;
            }
            else{
                return Cin_eDriverUnsupportedNumChannels;
            }
        case Cin_eFormatNUM_FORMATS:
            return Cin_eDriverInvalidFormat;
    }
    
    return Cin_eDriverInvalidFormat;
}

/*****************************************************************************/

enum Cin_DriverError Cin_DriverSupportsSampleRate(
    const struct Cin_Driver *drv,
    unsigned rate){
    
    assert(drv != NULL);
    assert(drv->ctx != NULL);
    assert(drv->dev != NULL);
    
    switch(rate){
        case 8000: /* FALLTHROUGH */
        case 11025: /* FALLTHROUGH */
        case 16000: /* FALLTHROUGH */
        case 22050: /* FALLTHROUGH */
        case 44100: /* FALLTHROUGH */
        case 48000:
            return Cin_eDriverSuccess;
        default:
            return Cin_eDriverUnsupportedSampleRate;
    
    }
}

/*****************************************************************************/

unsigned Cin_StructLoaderSize(){
    return sizeof(struct Cin_Loader);
}

/*****************************************************************************/

enum Cin_LoaderError Cin_CreateLoader(struct Cin_Loader *out,
    struct Cin_Driver *drv,
    unsigned sample_rate,
    unsigned num_channels,
    enum Cin_Format format){
    
    assert(out);
    assert(drv);
    assert(drv->ctx);
    assert(drv->dev);
#if 0
    {
        const enum Cin_LoaderError err =
            Cin_FormatCompatible(drv, sample_rate, num_channels, format);
        if(err != Cin_eLoaderSuccess)
            return err;
    }
#endif
    
    alcMakeContextCurrent(drv->ctx);
    
    out->snd.ctx = drv->ctx;
    
    {
        ALuint source;
        alGenSources(1, &source);
        out->snd.snd = source;
        
        alSourcef(source, AL_PITCH, 1.0f);
        alSourcef(source, AL_GAIN, 1.0f);
        alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        alSourcei(source, AL_LOOPING, AL_FALSE);
    }
    
    out->format = Cin_CinFormatToOpenALFormat(format, num_channels);
    out->channels = num_channels;
    out->sample_rate = sample_rate;
    
    return Cin_eLoaderSuccess;
}

/*****************************************************************************/

enum Cin_LoaderError Cin_LoaderPut(struct Cin_Loader *ld,
    const void *data,
    unsigned byte_size){
    
    ALuint buffer;
    alcMakeContextCurrent(ld->snd.ctx);
    if(!Cin_CleanOpenALSound(ld->snd.snd, &buffer)){
        alGenBuffers(1, &buffer);
    }
    alBufferData(buffer, ld->format, data, byte_size, ld->sample_rate);
    alSourceQueueBuffers(ld->snd.snd, 1, &buffer);
    
    return Cin_eLoaderSuccess;
}

/*****************************************************************************/

enum Cin_LoaderError Cin_LoaderFinalize(struct Cin_Loader *ld,
    struct Cin_Sound *out){
    
    ALuint buffer;
    alcMakeContextCurrent(ld->snd.ctx);
    if(Cin_CleanOpenALSound(ld->snd.snd, &buffer)){
        alDeleteBuffers(1, &buffer);
    }
    
    out->snd = ld->snd.snd;
    out->ctx = ld->snd.ctx;
    
    ld->snd.snd = 0;
    return Cin_eLoaderSuccess;
}

/*****************************************************************************/

unsigned Cin_StructSoundSize(){
    return sizeof(struct Cin_Sound);
}

/*****************************************************************************/

enum Cin_SoundError Cin_SoundPlay(struct Cin_Sound *snd){
    alcMakeContextCurrent(snd->ctx);
    alSourcei(snd->snd, AL_LOOPING, AL_FALSE);
    alSourcePlay(snd->snd);
    return Cin_eSoundSuccess;
}

/*****************************************************************************/

enum Cin_SoundError Cin_SoundStop(struct Cin_Sound *snd){
    alcMakeContextCurrent(snd->ctx);
    alSourceStop(snd->snd);
    alSourceRewind(snd->snd);
    return Cin_eSoundSuccess;
}

/*****************************************************************************/

void Cin_DestroySound(struct Cin_Sound *snd){
    alcMakeContextCurrent(snd->ctx);
    alDeleteSources(1, &(snd->snd));
}
