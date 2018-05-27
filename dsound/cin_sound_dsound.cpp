/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_sound_dsound.hpp"
#include "cin_driver_dsound.hpp"

#include <assert.h>
#include <new>
    
unsigned Cin_Sound::write(unsigned len, unsigned &wrote) {
    const LONG position = InterlockedExchange(&m_at, 0);
    
    void *dest0, *dest1;
    DWORD count0, count1;
    m_buffer->Lock(0, len, &dest0, &count0, &dest1, &count1, DSBLOCK_FROMWRITECURSOR);
    
    Cin_LoaderMemcpy(m_data, position, dest0, count0);
    
    Cin_LoaderMemcpy(m_data, position + count0, dest1, count1);
    
    m_buffer->Unlock(dest0, count0, dest1, count1);
    
    wrote = count0 + count1;
    InterlockedExchange(&m_at, position + count0 + count1);
    
    return position;
}

void Cin_Sound::setupFormat(unsigned sample_rate,
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

void Cin_Sound::setEvents(){

    IDirectSoundNotify8 *notify;
    const HRESULT query_result =
        m_buffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&notify);
    
    if(FAILED(query_result)){
        return;
    }
    
    assert(m_event != NULL);
    assert(notify != NULL);
    
    // Set up write notifications
    DSBPOSITIONNOTIFY notes[CIN_DSOUND_NOTIFY_COUNT + 2];
    const unsigned buffer_size = bufferSize();
    for(unsigned i = 0; i < CIN_DSOUND_NOTIFY_COUNT; i++){
        const unsigned offset = (i * buffer_size / CIN_DSOUND_NOTIFY_COUNT);
        notes[i].dwOffset = offset;
        notes[i].hEventNotify = m_event;
    }
    notes[CIN_DSOUND_NOTIFY_COUNT].dwOffset = DSBPN_OFFSETSTOP;
    notes[CIN_DSOUND_NOTIFY_COUNT].hEventNotify = m_event;
    
    notify->SetNotificationPositions(CIN_DSOUND_NOTIFY_COUNT + 1, notes);
    notify->Release();
}

Cin_Sound::Cin_Sound(struct Cin_Driver *drv,
    struct Cin_LoaderData *data,
    unsigned sample_rate,
    unsigned channels,
    enum Cin_Format format,
    unsigned byte_length)
  : m_dsound(Cin_GetDriver(drv))
  , m_driver(drv)
  , m_data(data)
  , m_at(0)
  , m_byte_length(byte_length){
    
    assert(drv != NULL);
    assert(data != NULL);
    // Setup the format.
    setupFormat(sample_rate, channels, format);
}

Cin_Sound::~Cin_Sound(){
    Cin_LoaderFreeData(m_data);
}
    
bool Cin_Sound::init(){
    // Create the buffer.
    
    DSBUFFERDESC descriptor;
    descriptor.dwSize = sizeof(DSBUFFERDESC);
    descriptor.dwFlags =
        DSBCAPS_CTRLPOSITIONNOTIFY |
        DSBCAPS_GETCURRENTPOSITION2 |
        DSBCAPS_GLOBALFOCUS;
    descriptor.dwBufferBytes = bufferSize();
    descriptor.dwReserved = 0;
    descriptor.lpwfxFormat = &m_fmt;
    descriptor.guid3DAlgorithm = DS3DALG_DEFAULT;

    const HRESULT result =
        m_dsound->CreateSoundBuffer(&descriptor, &m_buffer, NULL);
    
    assert(m_driver != NULL);
    
    if(SUCCEEDED(result)){
        // Fill the buffer.
        
        if(bufferSize() != m_byte_length){
            m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
            setEvents();
            m_driver->addSound(this, (uintptr_t)m_event);
        }
        else{
            for(unsigned i = 0; i < CIN_DSOUND_NOTIFY_COUNT; i++){
                m_event = NULL;
            }
            write(bufferSize());
        }
        
        return true;
    }
    else{
        return false;
    }
}

void Cin_Sound::onEvent(){

    if(m_die_at_next_event){
        m_buffer->Stop();
        return;
    }
    
    const unsigned to_write = (CIN_DSOUND_NOTIFY_COUNT < 3) ?
        min(m_byte_length - m_at, bufferSize()) :
        min(m_byte_length - m_at, (bufferSize() << 1) / CIN_DSOUND_NOTIFY_COUNT);
    
    const unsigned at = write(to_write);
    
    if(at >= m_byte_length){
        IDirectSoundNotify8 *notify;
        const HRESULT query_result =
            m_buffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&notify);
        
        if(FAILED(query_result)){
            puts("QUERY FAIL");
            return;
        }
        
        DSBPOSITIONNOTIFY note;
        note.dwOffset = DSBPN_OFFSETSTOP;
        note.hEventNotify = m_event;
        notify->SetNotificationPositions(1, &note);
        notify->Release();
        
        m_die_at_next_event = true;
    }
}

    
void Cin_Sound::play(){
    m_buffer->Play(0, 0, DSBPLAY_LOOPING);
    m_die_at_next_event = false;
}

void Cin_Sound::stop(){
    m_buffer->Stop();
    InterlockedExchange(&m_at, 0);
}

void Cin_Sound::kill(){
    stop();
    m_driver->removeSound(this);
}

unsigned Cin_StructSoundSize(){
    return sizeof(struct Cin_Sound);
}

enum Cin_LoaderError Cin_LoaderFinalize(struct Cin_Loader *ld,
    struct Cin_Sound *out){
    
    assert(ld != NULL);
    Cin_Driver *const drv = (Cin_Driver*)ld->data;
    assert(drv != NULL);
    
    new (out) Cin_Sound(drv,
        ld->first,
        ld->sample_rate,
        ld->channels,
        ld->format,
        ld->bytes_placed);
    
#ifndef NDEBUG
    /* Poison the loader */
    memset(ld, 0xFF, sizeof(struct Cin_Loader));
#endif
    
    if(out->init())
        return Cin_eLoaderSuccess;
    else
        return Cin_eLoaderFailure;
}

void Cin_DestroySound(struct Cin_Sound *snd){
    snd->kill();
}

enum Cin_SoundError Cin_SoundPlay(struct Cin_Sound *snd){
    snd->play();
    return Cin_eSoundSuccess;
}

enum Cin_SoundError Cin_SoundStop(struct Cin_Sound *snd){
    snd->stop();
    return Cin_eSoundSuccess;
}
