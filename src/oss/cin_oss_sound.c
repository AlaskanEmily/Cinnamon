/* Copyright (c) 2019 Alaskan Emily, Transnat Games
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_oss_sound.h"

#include "cin_oss_driver.h"
#include "cin_mixer_sound.h"
#include "cin_soft_loader.h"

#include "cin_dsp.h"
#include "cinnamon.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************/
/* Wraps up how most commands are implemented. */
static enum Cin_SoundError cin_sound_command(struct Cin_Sound *snd, int type){
    
    enum Cin_SoundError ret = Cin_eSoundSuccess;
    struct Cin_OSS_Command *cmd = Cin_OSS_NewCommand(snd->sound_id, type);
    
    if(Cin_OSS_Lock(snd->drv) != 0){
        assert(!"Could not lock");
        return Cin_eSoundFailure;
    }
    
    if(Cin_OSS_PushCommand(snd->drv, cmd) != 0){
        assert(!"Could not push command");
        ret = Cin_eSoundFailure;
    }
    
    if(Cin_OSS_Unlock(snd->drv) != 0){
        assert(!"Could not unlock");
        return Cin_eSoundFailure;
    }
    else{
        return ret;
    }
}

/*****************************************************************************/

enum Cin_LoaderError Cin_CreateLoader(struct Cin_Loader *out,
    struct Cin_Driver *drv,
    unsigned sample_rate,
    unsigned channels,
    enum Cin_Format format){
    
    Cin_CreateSoftLoader(out, sample_rate, channels, format);
    out->data = drv;
    
    return Cin_eLoaderSuccess;
}


/*****************************************************************************/

enum Cin_LoaderError Cin_LoaderFinalize(struct Cin_Loader *ld,
    struct Cin_Sound *out){
    
    struct Cin_Driver *const drv = ld->data;
    enum Cin_LoaderError err = Cin_eLoaderSuccess;
    CIN_DSP_NEW_STACK_FMT_STRUCT(drv_format);
    CIN_DSP_MIXER_FORMAT_SET(drv_format,
        Cin_OSS_GetFormat(drv),
        Cin_OSS_GetSampleRate(drv),
        Cin_OSS_GetNumChannels(drv));
    CIN_MIXER_SOUND_FROM_SOFT_LOADER(ld, drv_format, out->snd, malloc);
    CIN_DSP_FREE_STACK_FMT_STRUCT(drv_format);
    Cin_OSS_Lock(drv);
    {
        /* ID generation. */
        const unsigned short id = Cin_OSS_NewID(drv);
        
        /* Add the sound. */
        struct Cin_OSS_Command *const cmd =
            CIN_OSS_INSERT_SOUND_CMD(id, out->snd);
        Cin_OSS_PushCommand(drv, cmd);
        
        /* Store the ID */
        out->sound_id = id;
    }
    out->drv = drv;
cin_loader_fail:
    Cin_OSS_Unlock(drv);
    return err;
}

/*****************************************************************************/

unsigned Cin_StructSoundSize(){
    return sizeof(struct Cin_Sound);
}

/*****************************************************************************/

enum Cin_SoundError Cin_SoundPlay(struct Cin_Sound *snd){
    return cin_sound_command(snd, CIN_OSS_PLAY);
}

/*****************************************************************************/

enum Cin_SoundError Cin_SoundStop(struct Cin_Sound *snd){
    return cin_sound_command(snd, CIN_OSS_STOP);
}

/*****************************************************************************/

void Cin_DestroySound(struct Cin_Sound *snd){
    enum Cin_SoundError err = cin_sound_command(snd, CIN_OSS_DELETE);
    assert(err == Cin_eSoundSuccess);
    (void)err;
}

/*****************************************************************************/
