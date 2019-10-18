// Copyright (c) 2018 AlaskanEmily
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "cin_dsound_driver.hpp"
#include "cin_dsound_sound.hpp"
#include "cin_soft_loader.h"
#include "cinnamon.h"

#include <new>

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(unsigned) Cin_StructDriverSize(void){
    return sizeof(Cin_Driver);
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(enum Cin_DriverError) Cin_CreateDriver(struct Cin_Driver *drv){
    new (drv) Cin_Driver();
    return Cin_eDriverSuccess;
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(void) Cin_DestroyDriver(struct Cin_Driver *drv){
    drv->~Cin_Driver();
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(enum Cin_DriverError) Cin_DriverSupportsFormat(
    const struct Cin_Driver *drv,
    enum Cin_Format format,
    unsigned num_channels){
    
    // TODO!
    return Cin_eDriverSuccess;
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(enum Cin_DriverError) Cin_DriverSupportsSampleRate(
    const struct Cin_Driver *drv,
    unsigned rate){
    
    // TODO!
    return Cin_eDriverSuccess;
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(unsigned) Cin_StructSoundSize(){
    return sizeof(Cin_Sound);
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(enum Cin_SoundError) Cin_SoundPlay(Cin_Sound *snd){
    snd->play(false);
    return Cin_eSoundSuccess;
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(enum Cin_SoundError) Cin_SoundPlayLoop(Cin_Sound *snd, int loop){
    snd->play(!!loop);
    return Cin_eSoundSuccess;
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(enum Cin_SoundError) Cin_SoundStop(Cin_Sound *snd){
    snd->stop();
    return Cin_eSoundSuccess;
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(void) Cin_DestroySound(Cin_Sound *snd){
    snd->~Cin_Sound();
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(enum Cin_LoaderError) Cin_CreateLoader(Cin_Loader *out,
    Cin_Driver *drv,
    unsigned sample_rate,
    unsigned channels,
    enum Cin_Format format){
    Cin_CreateSoftLoader(out, sample_rate, channels, format);
    out->data = drv;
    return Cin_eLoaderSuccess;
}

///////////////////////////////////////////////////////////////////////////////

CIN_EXPORT(enum Cin_LoaderError) Cin_LoaderFinalize(Cin_Loader *ld, Cin_Sound *out){
    Cin_Driver *drv = static_cast<Cin_Driver*>(ld->data);
    drv->createSound(out, *ld);
    Cin_LoaderFreeData(ld->first);
    return Cin_eLoaderSuccess;
}
