Cinnamon Audio Library
======================

NOTICE: THIS PROJECT HAS MOVED
==============================

Cinnamon is now developed at: https://osdn.net/projects/cinnamon-audio/
-----------------------------------------------------------------------

I am leaving this repo here, in its last state before I changed hosting.
All further work will occur elsewhere. I do not support github.

Summary
-------

Cinnamon is a cross-platform, lightweight audio library for fully buffered audio. It currently
wraps OpenAL and DirectSound, providing a simple interface to play audio with a minimum of external
dependencies. Further backends are possible (there is a partial OSS backend).

Implementation Notes
--------------------

Cinnamon is implemented in C for common code (soft loader, etc). Platforms usually either use C or
the most native language for that platform. For instance, Java via JNI on Android, Objective C for
OS X, or C++ on Windows.

Cinnamon does not contain any logic to load audio files. You might want to check out a library like
libsndfile for a simple method to load a variety of audio formats.

License
-------

Cinnamon is MPL 2.0 licensed, which means that any changes to existing files must be released under
the MPL 2.0, but new files (for instance new platform backends) can use different licenses. Any
pull requests containing new files will only be accepted if the code is licensed as MPL 2.0, zlib,
or public domain.

The mixer code is licensed under the zlib license.

API Usage
------------

You can check the sine_test.c source file for an example of playing audio using Cinnamon.

Most applications will do something similar to the following:

```
struct Cin_Driver *const driver = malloc(Cin_StructDriverSize());
if(driver == NULL || Cin_CreateDriver(driver) != Cin_eDriverSuccess){
    /* Handle error... */
    abort();
}

/* Load a sound into memory. Cinnamon doesn't provide this capability itself.
 * You can load the sound in parts, see below. */
unsigned sound_size;
const void *sound_data = LoadSound(&sound_size);

/* Create an audio Loader. This is used solely to load the sound, and then will
 * be used to create a playable sound. */
struct Cin_Loader *const loader = malloc(Cin_StructLoaderSize());
if(loader == NULL || Cin_CreateLoader(loader, driver, 44100, 2, Cin_eFormatS16) != Cin_eDriverSuccess){
    /* Handle error... */
    abort();
}

/* Load the sound. If the sound is loaded in parts (as is the case for manually
 * parsing some formats like Ogg) you can call this function multiple times to
 * append more data to the loader. */
Cin_LoaderPut(loader, sound_data, sound_size);

struct Cin_Sound *const sound = malloc(Cin_StructSoundSize());
if(sound == NULL || Cin_LoaderFinalize(loader, sound) != Cin_eLoaderSuccess){
    /* Handle error... */
    abort();
}

/* You can now play your sound. */
Cin_SoundPlay(sound);

/* Obviously Cin_SoundPlay doesn't block, so some other processing can occur
 * here... */

/* Clean up. It is safe to call Cin_SoundStop regardless of if the sound is
 * still playing or not. */
Cin_SoundStop(sound);

Cin_DestroySound(sound);
free(sound);
Cin_DestroyDriver(driver);
free(driver);
```
