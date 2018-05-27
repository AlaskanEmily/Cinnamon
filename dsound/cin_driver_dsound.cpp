/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_driver.h"
#include "cin_driver_dsound.hpp"
#include "cin_loader_soft.h"
#include "cin_common.h"
#include "cin_sound_dsound.hpp"
#include "cin_lock.h"
#include "cin_thread.h"
#include "cin_select.h"

#include <algorithm>
#include <vector>
#include <new>
    
void Cin_Driver::signalNewEvents(){
    SetEvent(m_handles.front());
}

void Cin_Driver::run(){
    while(true){
        const HANDLE *const handles = (HANDLE*)(&(m_handles.front()));
        const unsigned count = m_handles.size();
        const DWORD signal =
            WaitForMultipleObjects(count, handles, FALSE, INFINITE);
        
        if(signal == WAIT_OBJECT_0){
            if(!processEventQueue())
                return;
        }
        else{
            assert(signal > WAIT_OBJECT_0);
            assert(m_sounds.size() + 1 == m_handles.size());
            const size_t which = (signal - WAIT_OBJECT_0) - 1;
            assert(which < m_sounds.size());
            m_sounds[which]->onEvent();
        }
    }
}

void Cin_Driver::remove(const struct Cin_Sound *sound){
    const size_t num_sounds = m_sounds.size();
    assert(num_sounds + 1 == m_handles.size());
    assert(num_sounds != 0);
    
    {
        const std::vector<Cin_Sound*>::iterator iter =
            std::find(m_sounds.begin(), m_sounds.end(), sound);
        
        assert(iter != m_sounds.end());
        
        const size_t i = std::distance(m_sounds.begin(), iter);
        
        // Instead of doing an erase() which can cause a large amount of data
        // copying to shift the remaining length of the vector, we swap the
        // element that we are removing with the last element in the vector.
        {
            // Swap the sound locations
            struct Cin_Sound *&snd = m_sounds[i];
            delete snd;
            snd = m_sounds.back();
        }
        {
            // Swap the handle locations
            HANDLE &hnd = m_handles[i + 1];
            CloseHandle(hnd);
            hnd = m_handles.back();
        }
    }
    
    m_sounds.pop_back();
    m_handles.pop_back();
}

void Cin_Driver::add(struct Cin_Sound *sound, uintptr_t event){
    m_handles.push_back((HANDLE)event);
    m_sounds.push_back(sound);
}

Cin_Driver::Cin_Driver(){
    m_handles.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
    CoInitialize(NULL);
    {
        const HRESULT result = DirectSoundCreate8(NULL, &m_dsound, NULL);
        assert(SUCCEEDED(result));
    }
    {
        const HWND window = GetDesktopWindow();
         const HRESULT result =
            m_dsound->SetCooperativeLevel(window, DSSCL_PRIORITY);
        assert(SUCCEEDED(result));
    }
}
    
Cin_Driver::~Cin_Driver(){
    CoUninitialize();
    std::for_each(m_handles.begin(), m_handles.end(), CloseHandle);
}

unsigned Cin_StructDriverSize(){
    return sizeof(struct Cin_Driver);
}

enum Cin_DriverError Cin_CreateDriver(struct Cin_Driver *drv){
    new (drv) Cin_Driver();
    drv->start();
    return Cin_eDriverSuccess;
}

void Cin_DestroyDriver(struct Cin_Driver *drv){
    drv->stop();
    drv->~Cin_Driver();
}

enum Cin_DriverError Cin_DriverSupportsFormat(
    const struct Cin_Driver *drv,
    enum Cin_Format format,
    unsigned num_channels){
    
    assert(drv);
    (void)drv;
    
    if(num_channels != 1 && num_channels != 2)
        return Cin_eDriverUnsupportedNumChannels;
    
    switch(format){
        case Cin_eFormatS8: // FALLTHROUGH
        case Cin_eFormatS16: // FALLTHROUGH
        case Cin_eFormatFloat32: // FALLTHROUGH
        case Cin_eFormatULaw8:
            return Cin_eDriverSuccess;
        case Cin_eFormatFloat64: // FALLTHROUGH
        case Cin_eFormatS32:
            return Cin_eDriverUnsupportedFormat;
        default:
            return Cin_eDriverInvalidFormat;
    }
}

CIN_EXPORT(enum Cin_DriverError) Cin_DriverSupportsSampleRate(
    const struct Cin_Driver *drv,
    unsigned rate){
    
    assert(drv);
    (void)drv;
    
    switch(rate){
        case 8000:
        case 11025:
        case 22050:
        case 44100:
        case 44800:
        case 48000:
            return Cin_eDriverSuccess;
        default:
            return Cin_eDriverUnsupportedSampleRate;
    }
}

enum Cin_LoaderError Cin_CreateLoader(struct Cin_Loader *out,
    struct Cin_Driver *drv,
    unsigned sample_rate,
    unsigned num_channels,
    enum Cin_Format format){
    
    assert(out != NULL);
    assert(drv != NULL);
    
    {
        const enum Cin_LoaderError err =
            Cin_FormatCompatible(drv, sample_rate, num_channels, format);
        if(err != Cin_eLoaderSuccess)
            return err;
    }
    
    Cin_CreateSoftLoader(out, sample_rate, num_channels, format);
    out->data = drv;
    
    return Cin_eLoaderSuccess;
}


CIN_PRIVATE(CComPtr<IDirectSound8>&) Cin_GetDriver(struct Cin_Driver *drv){
    assert(drv != NULL);
    return drv->getDirectSound();
}
