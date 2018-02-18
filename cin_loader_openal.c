/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_loader.h"
#include "cin_driver.h"
#include "cin_openal.h"
#include "cin_common.h"

#include <assert.h>

unsigned Cin_StructLoaderSize(){
    return sizeof(struct Cin_Loader);
}

enum Cin_LoaderError Cin_CreateLoader(struct Cin_Loader *out,
    struct Cin_Driver *drv,
    unsigned sample_rate,
    unsigned num_channels,
    enum Cin_Format format){
    
    assert(out);
    assert(drv);
    assert(drv->ctx);
    assert(drv->dev);
    
    {
        const enum Cin_LoaderError err =
            Cin_FormatCompatible(drv, sample_rate, num_channels, format);
        if(err != Cin_eLoaderSuccess)
            return err;
    }
    
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

CIN_EXPORT(enum Cin_LoaderError) Cin_LoaderFinailize(struct Cin_Loader *ld,
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
