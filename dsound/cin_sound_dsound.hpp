/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_SOUND_DSOUND_HPP
#define CIN_SOUND_DSOUND_HPP
#pragma once

#include "cin_sound.h"
#include "cin_loader_soft.h"
#include "cin_format.h"
#include "cin_export.h"
#include "cin_common.h"

#include <dsound.h>
#include <mmreg.h>
#include <objbase.h>
#include <atlbase.h>
#include <assert.h>

#define CIN_DSOUND_NOTIFY_COUNT 4

struct Cin_Driver;

struct Cin_Sound{
private:
    WAVEFORMATEX m_fmt;
    DSBUFFERDESC m_descriptor;
    CComPtr<IDirectSoundBuffer> m_buffer;
    CComPtr<IDirectSound8> m_dsound;
    struct Cin_Driver *m_driver;
    HANDLE m_event;
    struct Cin_LoaderData *const m_data;
    volatile LONG m_at;
    bool m_die_at_next_event;
    
    static inline WORD FormatTag(enum Cin_Format format){
        switch(format){
            case Cin_eFormatS8:
            case Cin_eFormatS16:
            case Cin_eFormatS32:
                return WAVE_FORMAT_PCM;
            case Cin_eFormatFloat32:
            case Cin_eFormatFloat64:
                return WAVE_FORMAT_IEEE_FLOAT;
            case Cin_eFormatULaw8:
                return WAVE_FORMAT_MULAW;
            default:
                assert(0);
                return 0;
        }
    }
    
    inline unsigned bufferSize() {
        // Buffer size is 2 seconds
        const unsigned max = 
            m_fmt.nChannels *
            m_fmt.nSamplesPerSec *
            m_fmt.wBitsPerSample / 4;
        
        if(max < m_byte_length)
            return max;
        else
            return m_byte_length;
    }
    
    inline void setupDescriptor(){
        m_descriptor.dwSize = sizeof(DSBUFFERDESC);
        m_descriptor.dwFlags =
            DSBCAPS_CTRLPOSITIONNOTIFY |
            DSBCAPS_GETCURRENTPOSITION2 |
            DSBCAPS_GLOBALFOCUS;
        m_descriptor.dwBufferBytes = bufferSize();
        m_descriptor.dwReserved = 0;
        m_descriptor.lpwfxFormat = &m_fmt;
        m_descriptor.guid3DAlgorithm = DS3DALG_DEFAULT;
    }
    
    inline void setupFormat(unsigned sample_rate,
        unsigned channels,
        enum Cin_Format format){
            
#ifndef NDEBUG
        // Poison the uninitialized format
        memset(&m_fmt, 0xFF, sizeof(WAVEFORMATEX));
#endif
        
        // Set format tag
        m_fmt.wFormatTag = FormatTag(format);
        
        // Set channels
        assert(channels == 1 || channels == 2);
        m_fmt.nChannels = channels;
        
        // Set sample rate
        m_fmt.nSamplesPerSec = sample_rate;
        
        // Block align
        const unsigned align = channels * Cin_BytesPerSample(format);
        m_fmt.nBlockAlign = align;
        
        m_fmt.nAvgBytesPerSec = sample_rate * align;
        
        // Set bits per sample
        m_fmt.wBitsPerSample = Cin_BytesPerSample(format) * 8;
        
        m_fmt.cbSize = 0;
    }
    
    unsigned write(unsigned len, unsigned &wrote);
    inline unsigned write(const unsigned len){
        unsigned _;
        return write(len, _);
    }
    
    void setEvents();
    
public:
    
    const unsigned m_byte_length;
    
    Cin_Sound(struct Cin_Driver *drv,
        struct Cin_LoaderData *data,
        unsigned sample_rate,
        unsigned channels,
        enum Cin_Format format,
        unsigned byte_length);
    
    ~Cin_Sound();
    
    bool init();
    
    inline void play(){
        m_buffer->Play(0, 0, DSBPLAY_LOOPING);
        m_die_at_next_event = false;
    }
    
    inline void stop(){
        m_buffer->Stop();
        InterlockedExchange(&m_at, 0);
    }
    
    void onEvent();
    
};

CIN_PRIVATE(CComPtr<IDirectSound8>&) Cin_GetDriver(struct Cin_Driver *);

CIN_PRIVATE(void) Cin_AddEvent(struct Cin_Driver *drv,
    Cin_Sound *snd,
    HANDLE event);

CIN_PRIVATE(void) Cin_RemoveEvent(struct Cin_Driver *drv,
    const Cin_Sound *snd);

#endif // CIN_SOUND_DSOUND_HPP
