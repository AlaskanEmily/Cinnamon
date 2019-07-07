// Copyright (c) 2018 AlaskanEmily
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "cin_dsound_sound.hpp"
#include "cinnamon.h"
#include "cin_soft_loader.h"

#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>

///////////////////////////////////////////////////////////////////////////////

Cin_Sound::Cin_Sound(IDirectSound8 *dsound, const Cin_Loader &ld)
  : m_dsound(dsound){

    const unsigned sample_rate = ld.sample_rate;
    const unsigned channels = ld.channels;
    const enum Cin_Format format = ld.format;

    WAVEFORMATEXTENSIBLE fmt;
#ifndef NDEBUG
    // Poison the uninitialized format
    memset((void*)&fmt, 0xFF, sizeof(fmt));
#endif

    // Set format tag
    switch(format){
        case Cin_eFormatS8:
        case Cin_eFormatS16:
        case Cin_eFormatS32:
            fmt.Format.cbSize = 0;
            fmt.Format.wFormatTag = WAVE_FORMAT_PCM;
            break;
        case Cin_eFormatFloat32:
        case Cin_eFormatFloat64:
        case Cin_eFormatULaw8:
            fmt.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
            fmt.Format.cbSize = sizeof(fmt) - sizeof(fmt.Format);
            break;
    }

    unsigned bytes_per_sample = 0;
    switch(format){
        case Cin_eFormatS8:
            bytes_per_sample = 1;
            break;
        case Cin_eFormatS16:
            bytes_per_sample = 2;
            break;
        case Cin_eFormatS32:
            bytes_per_sample = 4;
            break;
        case Cin_eFormatFloat32:
            bytes_per_sample = 4;
            fmt.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
            fmt.Samples.wValidBitsPerSample = 32;
            break;
        case Cin_eFormatFloat64:
            bytes_per_sample = 8;
            fmt.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
            fmt.Samples.wValidBitsPerSample = 64;
            break;
        case Cin_eFormatULaw8:
            bytes_per_sample = 1;
            fmt.SubFormat = KSDATAFORMAT_SUBTYPE_MULAW;
            fmt.Samples.wValidBitsPerSample = 8;
            break;
    }

    // Set channels
    fmt.Format.nChannels = channels;
    if(fmt.Format.wFormatTag != WAVE_FORMAT_PCM){
        switch(channels){
            case 1:
                fmt.dwChannelMask = KSAUDIO_SPEAKER_MONO;
                break;
            case 2:
                fmt.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
                break;
            case 4:
                fmt.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
                break;
            case 6:
                fmt.dwChannelMask = KSAUDIO_SPEAKER_5POINT1_SURROUND;
                break;
            default:
                fmt.dwChannelMask = 0;
        }
    }

    // Set sample rate
    fmt.Format.nSamplesPerSec = sample_rate;

    // Block align
    const unsigned align = channels * bytes_per_sample;
    const unsigned bytes_per_sec = align * sample_rate;
    fmt.Format.nBlockAlign = align;

    fmt.Format.nAvgBytesPerSec = bytes_per_sec;

    // Set bits per sample
    fmt.Format.wBitsPerSample = bytes_per_sample << 3;

    // Check the size of the loader data.
    const unsigned byte_size = ld.bytes_placed;
#ifndef NDEBUG
    {
        unsigned debug_byte_size = 0;
        for(const struct Cin_LoaderData *i = ld.first; i != NULL; i = i->next){
            debug_byte_size += i->len;
        }
        assert(debug_byte_size == byte_size);
    }
#endif

    DSBUFFERDESC descriptor;
    descriptor.dwSize = sizeof(DSBUFFERDESC);
    descriptor.dwFlags = DSBCAPS_GLOBALFOCUS;
    descriptor.dwBufferBytes = byte_size;
    descriptor.dwReserved = 0;
    descriptor.lpwfxFormat = &fmt.Format;
    descriptor.guid3DAlgorithm = DS3DALG_DEFAULT;

    void* buffer_data;
    DWORD buffer_size;

    if(m_dsound->CreateSoundBuffer(&descriptor, &m_buffer, NULL) != DS_OK){
        m_dsound->Release();
        m_buffer = NULL;
    }
    else if(m_buffer->Lock(0, 0, &buffer_data, &buffer_size, 0, 0, DSBLOCK_ENTIREBUFFER) == DS_OK &&
        buffer_size == byte_size){
        
        Cin_LoaderMemcpy(ld.first, 0, buffer_data, byte_size);
        m_buffer->Unlock(buffer_data, byte_size, NULL, 0);
    }
    else{
        m_dsound->Release();
        m_buffer->Release();
        m_buffer = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////

Cin_Sound::~Cin_Sound(){
    if(m_buffer != NULL){
        m_buffer->Release();
        m_dsound->Release();
    }
}

///////////////////////////////////////////////////////////////////////////////

void Cin_Sound::play(bool loop){
    if(m_buffer != NULL)
        m_buffer->Play(0, 0, loop ? DSBPLAY_LOOPING : 0);
}

///////////////////////////////////////////////////////////////////////////////

void Cin_Sound::stop(){
    if(m_buffer != NULL)
        m_buffer->Stop();
}

///////////////////////////////////////////////////////////////////////////////
