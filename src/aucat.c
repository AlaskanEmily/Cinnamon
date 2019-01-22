/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#ifdef _MSC_VER
/* Fart off, Visual C++ */
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "cinnamon.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * This demonstrates the very basics of loading a sound from a file and playing
 * it. Cinnamon is NOT a decoding library, so we load raw sounds only. You can
 * create these with ffmpeg if you want to test. In OSS you can just use dd on
 * the microphone file to get some data.
 */

/* The platform deps are just for sleeping while the sound plays.
 * Define AUCAT_SLEEP as a macro of arity 1 that will sleep for the argument
 * number of milliseconds.
 */
#ifdef _WIN32
/* On Windows, Sleep does exactly what we want AUCAT_SLEEP to do. */

#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#define AUCAT_SLEEP Sleep

#elif defined __linux__
/* usleep is missing/deprecated on Linux, use nanosleep instead. */

#include <time.h>
#include <unistd.h>

#define AUCAT_SLEEP(MS) do{\
    const unsigned long AUCAT_SLEEP_ms = (MS);\
    struct AUCAT_SLEEP_ts;\
    AUCAT_SLEEP_ts.tn_sec = (AUCAT_SLEEP_ms % 1000) * 1000000;\
    AUCAT_SLEEP_ts.tv_sec = AUCAT_SLEEP_ms / 1000;\
    nanosleep(&AUCAT_SLEEP_ts, NULL);\
} while(0)

#else
/* On BSD and OS X (and likely other platforms like Solaris and AIX) we can
 * simply use nanosleep. This requires _BSD_SOURCE on some platforms.
 */

#define _BSD_SOURCE
#include <time.h>
#include <unistd.h>

#define AUCAT_SLEEP(MS) do{ nanosleep((MS) * 1000); }while(0)

#endif

int main(int argc, char **argv){
	FILE *const input = (argc < 2) ?
		stdin : fopen(argv[1], "rb");
	
    int channels = 2;
    int rate = 44100;
    enum Cin_Format format = Cin_eFormatS16;
    
    /* Read options. */
    int i;
    for(i = 2; i + 1 < argc; i++){
        if(strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--channels") == 0){
            channels = atoi(argv[++i]);
        }
        else if(strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--rate") == 0){
            rate = atoi(argv[++i]);
        }
        else if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0){
            
            i++;
            if(strcmp(argv[i], "16") == 0 || strcmp(argv[i], "s16") == 0){
                format = Cin_eFormatS16;
            }
            else if(strcmp(argv[i], "8") == 0 || strcmp(argv[i], "s8") == 0){
                format = Cin_eFormatS8;
            }
            else if(strcmp(argv[i], "32") == 0 || strcmp(argv[i], "s32") == 0){
                format = Cin_eFormatS32;
            }
            else if(strcmp(argv[i], "float") == 0 || strcmp(argv[i], "float32") == 0){
                format = Cin_eFormatFloat32;
            }
            else if(strcmp(argv[i], "double") == 0 || strcmp(argv[i], "float64") == 0){
                format = Cin_eFormatFloat64;
            }
            else if(strcmp(argv[i], "u") == 0 || strcmp(argv[i], "ulaw") == 0){
                format = Cin_eFormatULaw8;
            }
            else{
                fprintf(stderr, "Invalid format: %s\n", argv[i]);
                return EXIT_FAILURE;
            }
        }
    }
    
    /* Validate options. */
    if(channels == 0 || channels > 8){
		fprintf(stderr, "Invalid number of channels: %i\n", channels);
		return EXIT_FAILURE;
    }
    
    if(rate < 8000 || rate > 0x00FFFFFF){
		fprintf(stderr, "Invalid sample rate: %i\n", rate);
		return EXIT_FAILURE;
    }
    
    /* Validate we could open the file. */
    if(input == NULL){
		fputs("Could not open file", stderr);
        if(argc >= 2)
            fputs(argv[i], stderr);
        fputc('\n', stderr);
		return EXIT_FAILURE;
	}
    
    /* Cat the file. */
	{
		char *const buffer = malloc(rate);
		unsigned num_read, total_read = 0;
		
		struct Cin_Driver *const driver = malloc(Cin_StructDriverSize());
		struct Cin_Sound *const sound = malloc(Cin_StructSoundSize());
		struct Cin_Loader *const loader = malloc(Cin_StructLoaderSize());
		
		if(driver == NULL || Cin_CreateDriver(driver) != Cin_eDriverSuccess){
			fputs("Could not create driver\n", stderr);
			return EXIT_FAILURE;
		}
		
		if(loader == NULL || Cin_CreateLoader(loader, driver, rate, channels, format) != Cin_eLoaderSuccess){
            fputs("Could not create loader\n", stderr);
			return EXIT_FAILURE;
		}
		
		while(num_read = fread(buffer, 1, rate, input)){
			Cin_LoaderPut(loader, buffer, num_read);
            total_read += num_read;
		}
        
        /* The buffer is no longer needed. */
        free(buffer);
        
        if(sound == NULL || Cin_LoaderFinalize(loader, sound) != Cin_eLoaderSuccess){
            fputs("Could not create sound\n", stderr);
			return EXIT_FAILURE;
        }
        
        /* Loader is totally done. */
        free(loader);
        
        /* Play the sound */
        Cin_SoundPlay(sound);
        
        /* Sleep while the sound is playing. */
        AUCAT_SLEEP(((total_read * 1000) + 1) / (rate * channels));
        
        /* This is technically not needed, but it will catch if the sound
         * has playback issues on some systems where DestroyDriver blocks until
         * all sounds are complete.
         */
        Cin_SoundStop(sound);
        
        Cin_DestroySound(sound);
        
        free(sound);
	}
}
