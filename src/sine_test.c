/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#define _POSIX_C_SOURCE 199309L

#include "cinnamon.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Get a sleep function. */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <time.h>
#include <unistd.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265f
#endif

int main(int argc, char **argv){
    struct Cin_Driver *const driver = malloc(Cin_StructDriverSize());
    if(driver == NULL){
        puts("Could not allocate driver.");
        return EXIT_FAILURE;
    }
    
    (void)argc;
    (void)argv;
    
    assert(Cin_StructDriverSize() > 0);
    assert(Cin_StructLoaderSize() > 0);
    assert(Cin_StructSoundSize() > 0);
    
    if(driver == NULL || Cin_CreateDriver(driver) == Cin_eDriverSuccess){
        struct Cin_Loader *const loader = malloc(Cin_StructLoaderSize());
        struct Cin_Sound *const sound = malloc(Cin_StructSoundSize());
        
        /* TODO: Make these configurable */
        unsigned num_channels = 2,
            sample_rate = 48000,
            sin_frequency = 440,
            num_seconds = 11;
        
        /* Times two for the size of int16_t */
        const unsigned data_size = sample_rate * num_channels << 1;
        short *const data = malloc(data_size);
        
        if(!(loader != NULL && sound != NULL && data != NULL)){
            if(!loader)
                puts("Could not allocate loader.");
            if(!sound)
                puts("Could not allocate sound.");
            if(!data)
                puts("Could not allocate buffer for generating audio.");
            free(loader);
            free(sound);
            free(data);
            Cin_DestroyDriver(driver);
            free(driver);
            return EXIT_FAILURE;
        }
        
        {
            const enum Cin_LoaderError err =
                Cin_CreateLoader(loader,
                    driver,
                    sample_rate,
                    num_channels,
                    Cin_eFormatS16);
            if(err != Cin_eLoaderSuccess){
                free(loader);
                free(sound);
                free(data);
                Cin_DestroyDriver(driver);
                free(driver);
                puts("Could not create loader.");
                return EXIT_FAILURE;
            }
        }
        
        {
            const float multiplier = M_PI * 2 * (float)sin_frequency;
            unsigned i;
            /* Create one second of 44100 Hz audio */
            /* We want to cycle sequence 440 times a second. */
            for(i = 0; i < sample_rate; i++){
                const float sequence =
                    multiplier * (float)i / (float)sample_rate;
                const float sample = sinf(sequence) * (float)(0xFFFF >> 1);
                const unsigned at = i * num_channels;
                data[at] = data[at + 1] = (short)(sample * 0.1);
            }
            
            /* Put the second once for each second of data we want to play. */
            for(i = 0; i < num_seconds; i++){
                Cin_LoaderPut(loader, data, data_size);
            }
        }
        
        /* Create a new sound from this loader. */
        {
            const enum Cin_LoaderError err =
                Cin_LoaderFinalize(loader, sound);
            
            free(loader);
            free(data);
            
            if(err != Cin_eLoaderSuccess){
                free(sound);
                Cin_DestroyDriver(driver);
                free(driver);
                puts("Could not create sound.");
                return EXIT_FAILURE;
            }
        }
        
        puts("Playing");
        Cin_SoundPlay(sound);
        
#ifdef _WIN32
        Sleep((1000 * num_seconds) + 1);
#else
        {
            struct timespec t;
            t.tv_sec = num_seconds;
            t.tv_nsec = 1;
            nanosleep(&t, NULL);
        }
#endif
        
        Cin_SoundStop(sound);
        Cin_DestroySound(sound);
        free(sound);
    }
    else{
        puts("Could not create driver.");
        return EXIT_FAILURE;
    }
    
    Cin_DestroyDriver(driver);
    free(driver);
    return EXIT_SUCCESS;
}
