/* Copyright (c) 2019 Alaskan Emily, Transnat Games
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_oss_driver.h"
#include "cin_oss_sound.h"
#include "cin_oss.h"
#include "cinnamon.h"

#include <stdlib.h>
#include <assert.h>

#include <sys/soundcard.h>

/* HACK!!! This detects BSD's with an OSS wrapper... */
#ifndef ioctl
#include <sys/ioctl.h>
#endif

#include <sys/queue.h>

#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

/*****************************************************************************/
/* Used to represent the sound inside the OSS driver code. */
struct Cin_OSS_Sound {
    struct Cin_MixerSound *snd;
    unsigned short sound_id;
    SLIST_ENTRY(Cin_OSS_Sound) entries;
};

/*****************************************************************************/

SLIST_HEAD(Cin_OSS_SoundList, Cin_OSS_Sound);

/*****************************************************************************/

struct Cin_OSS_Command {
    TAILQ_ENTRY(Cin_OSS_Command) entries;
    struct Cin_MixerSound *snd;
    unsigned short sound_id;
    unsigned char command;
};

/*****************************************************************************/

TAILQ_HEAD(Cin_OSS_CommandList, Cin_OSS_Command);

/*****************************************************************************/

struct Cin_Driver {
    pthread_t thread;
    
    pthread_mutex_t lock;
    struct Cin_OSS_CommandList commands;
    struct Cin_OSS_SoundList sounds;
    struct Cin_MixerSound *channels[CIN_OSS_SOUND_CHANNELS];
    unsigned rate;
    int dev;
    unsigned char num_channels, format;
    unsigned short id_counter;
    
    
};

/*****************************************************************************/

CIN_PRIVATE(Cin_OSS_Command_Ptr) Cin_OSS_NewCommand(int id, int cmd){
    struct Cin_OSS_Command *const command =
        malloc(sizeof(struct Cin_OSS_Command));
    
    assert(command != NULL);
    
    command->snd = NULL;
    command->sound_id = id;
    command->command = cmd;
    
    return command;
}

/*****************************************************************************/

CIN_PRIVATE(Cin_OSS_Command_Ptr) Cin_OSS_CommandInsertSound(
    struct Cin_OSS_Command *cmd,
    struct Cin_MixerSound *snd){
    
    cmd->snd = snd;
    return cmd;
}

/*****************************************************************************/

CIN_PRIVATE(int) Cin_OSS_Lock(struct Cin_Driver *drv){
    return pthread_mutex_lock(&(drv->lock));
}

/*****************************************************************************/

CIN_PRIVATE(int) Cin_OSS_Unlock(struct Cin_Driver *drv){
    return pthread_mutex_unlock(&drv->lock);
}

/*****************************************************************************/

CIN_PRIVATE(int) Cin_OSS_NewID(struct Cin_Driver *drv){
    return ++(drv->id_counter);
}

/*****************************************************************************/

CIN_PRIVATE(int) Cin_OSS_PushCommand(struct Cin_Driver *drv,
    struct Cin_OSS_Command *cmd){
    TAILQ_INSERT_TAIL(&drv->commands, cmd, entries);
    return 1;
}

/*****************************************************************************/
/* Used by the the thread backend to play channels. */
CIN_PRIVATE_PURE(Cin_MixerSound_PtrPtr) Cin_OSS_GetChannels(
    const struct Cin_Driver *drv){

    return (Cin_MixerSound_PtrPtr)drv->channels;
}

/*****************************************************************************/

CIN_PRIVATE_PURE(unsigned) Cin_OSS_GetSampleRate(const struct Cin_Driver *drv){
    return drv->rate;
}

/*****************************************************************************/

CIN_PRIVATE_PURE(int) Cin_OSS_GetDevice(const struct Cin_Driver *drv){
    return drv->dev;
}

/*****************************************************************************/

CIN_PRIVATE_PURE(int) Cin_OSS_GetNumChannels(const struct Cin_Driver *drv){
    return drv->num_channels;
}

/*****************************************************************************/

CIN_PRIVATE_PURE(int) Cin_OSS_GetFormat(const struct Cin_Driver *drv){
    return drv->format;
}

/*****************************************************************************/
/* As with all thread functions, the driver must NOT be locked when called. */
CIN_PRIVATE(int) Cin_OSS_ProcessCommands(struct Cin_Driver *drv){
    
    struct Cin_OSS_CommandList commands;
    /* Swap the command queues while locked. */
    {
        pthread_mutex_lock(&drv->lock);
        
        commands = drv->commands;
        TAILQ_INIT(&drv->commands);
        
        pthread_mutex_unlock(&drv->lock);
    }
    
    {
        int val = 0;
        /* Process the command queue. */
        struct Cin_OSS_Command *cmd = TAILQ_FIRST(&commands);
        while(cmd != NULL){
            unsigned i;
            struct Cin_OSS_Sound *sound = NULL, *prev = NULL;
            const unsigned char command = cmd->command;
            const unsigned short sound_id = cmd->sound_id;
            
            if(command == CIN_OSS_INSERT){
                sound = calloc(sizeof(struct Cin_OSS_Sound), 1);
                sound->sound_id = sound_id;
                sound->snd = cmd->snd;
            }
            else{
                struct Cin_OSS_Sound *iter;
                SLIST_FOREACH(iter, &drv->sounds, entries){
                    if(iter->sound_id == sound_id){
                        sound = iter;
                        break;
                    }
                    prev = iter;
                }
            }
            
            {
                struct Cin_OSS_Command *tmp = TAILQ_NEXT(cmd, entries);
                free(cmd);
                cmd = tmp;
            }
            
            switch(command){
                case CIN_OSS_INSERT:
                    puts("CIN_OSS_INSERT");
                    SLIST_INSERT_HEAD(&drv->sounds, sound, entries);
                    break;
                case CIN_OSS_DELETE:
                    puts("CIN_OSS_DELETE");
                    if(prev == NULL)
                        SLIST_REMOVE_HEAD(&drv->sounds, entries);
                    else
                        SLIST_REMOVE_AFTER(prev, entries);
                    /* We want to stop any playing instances of this sound. */
                    
                    /* FALLTHROUGH */
                case CIN_OSS_STOP:
                    if(command == CIN_OSS_STOP)
                        puts("CIN_OSS_STOP");
                    for(i = 0; i < CIN_OSS_SOUND_CHANNELS; i++){
                        if(drv->channels[i] == sound->snd){
                            drv->channels[i] = NULL;
                        }
                    }
                    break;
                case CIN_OSS_PLAY:
                    puts("CIN_OSS_PLAY");
                    for(i = 0; i < CIN_OSS_SOUND_CHANNELS; i++){
                        if(drv->channels[i] == NULL){
                            drv->channels[i] = sound->snd;
                            break;
                        }
                    }
                    /* TODO: Actually decide to evict sounds on if there are
                     * too many playing. We could also put this in some extra
                     * list of sounds and then play it if subsequent stop
                     * commands freed up more channels.
                     */
                    break;
                case CIN_OSS_QUIT:
                    puts("CIN_OSS_QUIT");
                    /* Keep going so that we at least drain the queue. */
                    val = 1;
                    break;
            }
        }
        return val;
    }
}

/*****************************************************************************/

static enum Cin_Format cin_oss_format(int fmt){
        switch(fmt){
            case AFMT_S16_LE:
                return Cin_eFormatS16;
#ifdef AFMT_S8_LE
            case AFMT_S8_LE:
                return Cin_eFormatS8;
#endif
                
                /* Enable once cin_mixer has mu-law support? */
            /*
            case AFMT_MU_LAW:
                return Cin_eFormatULaw8;
            */
            default:
                assert(0 && "Invalid format");
                return Cin_eFormatNUM_FORMATS;
        }

}

/*****************************************************************************/

unsigned Cin_StructDriverSize(void){
    return sizeof(struct Cin_Driver);
}

/*****************************************************************************/

enum Cin_DriverError Cin_CreateDriver(struct Cin_Driver *drv){
    enum Cin_DriverError ret = Cin_eDriverSuccess;
    
    TAILQ_INIT(&drv->commands);
    SLIST_INIT(&drv->sounds);
    
    {
        int err = pthread_mutex_init(&drv->lock, NULL);
        if(err != 0){
            ret = Cin_eDriverFailure;
            goto fail;
        }
    }
    
    bzero(drv->channels, sizeof(drv->channels));
    drv->id_counter = 0;
    
    {
        int dev = -1, val = 0;
        const char *const devname = getenv("AUDIODEV");
        if(devname != NULL)
            dev = open(devname, O_WRONLY);
        
        if(dev == -1 && devname == NULL)
            dev = open("/dev/dsp", O_WRONLY);
        
        if(dev == -1){
            ret = Cin_eDriverNoDevice;
            goto fail_mutex;
        }
        
        drv->dev = dev;
        
        /* Setup the format. Ask for the most basic format, although we do
         * support some others.
         */
        val = AFMT_S16_LE;
        if(ioctl(dev, SNDCTL_DSP_SETFMT, &val) == -1){
            ret = Cin_eDriverFailure;
            goto fail_device;
        }
        
        {
            const enum Cin_Format fmt = cin_oss_format(val);
            if(fmt == Cin_eFormatNUM_FORMATS){
                ret = Cin_eDriverUnsupportedFormat;
                goto fail_device;
            }
            drv->format = fmt;
        }
        
        /* Ask for stereo, although we accept mono. */
        val = 1;
        if(ioctl(dev, SNDCTL_DSP_STEREO, &val) == -1){
            drv->num_channels = 1;
        }
        else if(val == 1 || val == 2 || val == 4){
            drv->num_channels = val;
        }
        else{
            ret = Cin_eDriverUnsupportedNumChannels;
            goto fail_device;
        }
        
        /* Ask for opus-native frequency. This could easily become 44100, but
         * we'll deal with that if it happens.
         */
        val = 48000;
        if(ioctl(dev, SNDCTL_DSP_SPEED, &val) == -1){
            ret = Cin_eDriverFailure;
            goto fail_device;
        }
        
        if(val < 8000 || val > 198000){
            ret = Cin_eDriverUnsupportedSampleRate;
            goto fail_device;
        }
        
        /* Apparently this kind of thing can commonly happen on IRIX... */
        if(val >= 42000 && val < 46000)
            val = 44100;
        else if(val >= 46000 && val <= 50000)
            val = 48000;
        
        drv->rate = val;
        
        /* Set the buffer size. */
        val = 0x0004000B;
        if(ioctl(dev, SNDCTL_DSP_SETFRAGMENT, &val) == -1){
            ret = Cin_eDriverFailure;
            goto fail_device;
        }
    }
    
    {
        /* All systems go, create the mixer thread */
        int err = pthread_create(&drv->thread, NULL, Cin_OSS_ThreadFunc, drv);
        if(err != 0){
            ret = Cin_eDriverFailure;
            goto fail_device;
        }
    }
    
    return ret;
fail_device:
    close(drv->dev);
fail_mutex:
    pthread_mutex_destroy(&drv->lock);
fail:
    return ret;
}

/*****************************************************************************/

void Cin_DestroyDriver(struct Cin_Driver *drv){
    /* Queue up a CIN_OSS_QUIT command. */
    const Cin_OSS_Command_Ptr quit = Cin_OSS_NewCommand(0, CIN_OSS_QUIT);
    assert(quit != NULL);
    Cin_OSS_PushCommand(drv, quit);
    {
        void *unused;
        pthread_join(&drv->thread, &unused);
    }
    /* Finalize the driver. */
    close(drv->dev);
    pthread_mutex_destroy(&drv->lock);
}
