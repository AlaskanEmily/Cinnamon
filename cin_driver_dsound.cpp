/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_driver.h"
#include "cin_loader_soft.h"
#include "cin_common.h"
#include "cin_sound_dsound.hpp"

#include <dsound.h>
#include <objbase.h>
#include <atlbase.h>

#include <assert.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <new>

class Cin_DriverThreadData {
    // The first element is a event to signal that there is a new element to add.
    std::vector<HANDLE> m_handles;
    std::vector<Cin_Sound*> m_sounds;
    
    const HANDLE m_mutex;
    
    // If the HANDLE is NULL, it indicates we should remove the sound.
    std::vector<std::pair<HANDLE, Cin_Sound*> > m_to_add;
    
    bool m_was_started;
    HANDLE m_thread;
    
public:
    
    Cin_DriverThreadData(bool start = false)
      : m_mutex(CreateMutex(NULL, FALSE, NULL))
      , m_was_started(false)
      , m_thread(NULL){
        assert(m_mutex != NULL);
        if(start){
            startThread();
        }
    }
    
    ~Cin_DriverThreadData(){
        if(m_thread != NULL){
            setDie();
            WaitForSingleObject(m_thread, INFINITE);
            CloseHandle(m_thread);
        }
        CloseHandle(m_mutex);
    }
    
    void addEvent(HANDLE hnd, Cin_Sound *snd){
        assert(snd != NULL);
        assert(hnd != NULL);
        if(snd == NULL || hnd == NULL)
            return;
        
        WaitForSingleObject(m_mutex, INFINITE);
        m_to_add.push_back(std::pair<HANDLE, Cin_Sound*>(hnd, snd));
        
        if(!m_was_started)
            startThread();
        
        SetEvent(m_handles[0]);
        ReleaseMutex(m_mutex);
    }
    
    void removeEvent(const Cin_Sound *snd){
        assert(snd != NULL);
        if(snd == NULL)
            return;
        WaitForSingleObject(m_mutex, INFINITE);
        m_to_add.push_back(std::pair<HANDLE, Cin_Sound*>(NULL, (Cin_Sound *)snd));
        SetEvent(m_handles.front());
        ReleaseMutex(m_mutex);
    }
    
    void setDie(){
        WaitForSingleObject(m_mutex, INFINITE);
        m_to_add.push_back(std::pair<HANDLE, Cin_Sound*>(NULL, NULL));
        SetEvent(m_handles.front());
        ReleaseMutex(m_mutex);
    }
    
    void run(){
        assert(m_handles.size() == 1);
cin_driver_thread_run:
        {
            HANDLE *const handles = &(m_handles[0]);
            const unsigned count = m_handles.size();
            assert(count == m_sounds.size());
            const DWORD signal =
                WaitForMultipleObjects(count, handles, FALSE, INFINITE);
            
            if(signal == WAIT_OBJECT_0){
                WaitForSingleObject(m_mutex, INFINITE);
                
                assert(!m_to_add.empty());
                
                do{
                    
                    const std::pair<HANDLE, Cin_Sound*> &to_add = m_to_add.back();
                    
                    if(to_add.second == NULL){
                        std::for_each(m_handles.begin(), m_handles.end(), CloseHandle);
                        ReleaseMutex(m_mutex);
                        return;
                    }
                    
                    if(to_add.first == NULL){
                        assert(count == m_sounds.size());
                        
                        std::vector<Cin_Sound*>::iterator snd_iter =
                            std::find(m_sounds.begin(), m_sounds.end(), to_add.second);
                        
                        assert(snd_iter != m_sounds.end());
                        
                        std::vector<HANDLE>::iterator hnd_iter =
                            m_handles.begin() + std::distance(m_sounds.begin(), snd_iter);
                        
                        // Swap in the final objects.
                        *hnd_iter = m_handles.back();
                        *snd_iter = m_sounds.back();
                        
                        m_handles.pop_back();
                        m_sounds.pop_back();
                    }
                    else{
                        m_handles.push_back(to_add.first);
                        m_sounds.push_back(to_add.second);
                    }
                    
                    assert(m_handles.size() == m_sounds.size());
                    
                    m_to_add.pop_back();
                    
                }while(!m_to_add.empty());
                
                ReleaseMutex(m_mutex);
            }
            else{
                m_sounds[signal - WAIT_OBJECT_0]->onEvent();
            }
        }
        goto cin_driver_thread_run;
    }
    
    static DWORD WINAPI ThreadProc(void *data){
        static_cast<Cin_DriverThreadData*>(data)->run();
        return 0;
    }
    
    void startThread(){
        assert(m_was_started == false);
        
        m_was_started = true;
        
        assert(m_handles.empty());
        assert(m_sounds.empty());
        
        m_handles.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
        m_sounds.push_back(NULL);
        
        m_thread = CreateThread(NULL, 0L, ThreadProc, this, 0, NULL);
    }
};

struct Cin_Driver{
    
    CComPtr<IDirectSound8> m_dsound;
    Cin_DriverThreadData m_thread;
    
    Cin_Driver(){
        CoInitialize(NULL);
    }
    
    ~Cin_Driver(){
        CoUninitialize();
    }
    
    bool init(){
        HRESULT result =
            DirectSoundCreate8(NULL, &m_dsound, NULL);
        if(SUCCEEDED(result)){
            const HWND window = GetDesktopWindow();
            result = m_dsound->SetCooperativeLevel(window, DSSCL_PRIORITY);
        }
        return SUCCEEDED(result);
    }
};

unsigned Cin_StructDriverSize(){
    return sizeof(struct Cin_Driver);
}

enum Cin_DriverError Cin_CreateDriver(struct Cin_Driver *drv){
    new (drv) Cin_Driver();
    if(drv->init())
        return Cin_eDriverSuccess;
    else
        return Cin_eDriverFailure;
}

void Cin_DestroyDriver(struct Cin_Driver *drv){
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
    
    assert(out);
    assert(drv);
    
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
    return drv->m_dsound;
}

CIN_PRIVATE(void) Cin_AddEvent(struct Cin_Driver *drv,
    Cin_Sound *snd,
    HANDLE hnd){
    
    drv->m_thread.addEvent(hnd, snd);
}

CIN_PRIVATE(void) Cin_RemoveEvent(struct Cin_Driver *drv,
    const Cin_Sound *snd){
    
    drv->m_thread.removeEvent(snd);
}
