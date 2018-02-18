/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_sound_dsound.hpp"

#include <assert.h>
#include <new>
    
void Cin_Sound::write(unsigned len) {
    LONG position = InterlockedExchange(&m_at, 0);
    void *dest0, *dest1;
    DWORD count0, count1;
    m_buffer->Lock(0, len, &dest0, &count0, &dest1, &count1, DSBLOCK_FROMWRITECURSOR);
    
    Cin_LoaderMemcpy(m_data, position, dest0, count0);
    position += count0;
    
    Cin_LoaderMemcpy(m_data, position, dest1, count1);
    position += count1;
    
    m_buffer->Unlock(dest0, count0, dest1, count1);
    InterlockedExchange(&m_at, position);
}

Cin_Sound::Cin_Sound(CComPtr<IDirectSound8> &dsound,
    struct Cin_LoaderData *data,
    unsigned sample_rate,
    unsigned channels,
    enum Cin_Format format,
    unsigned byte_length)
  : m_dsound(dsound)
  , m_data(data)
  , m_at(0)
  , m_byte_length(byte_length){
  
    // Setup the format.
    setupFormat(sample_rate, channels, format);
    setupDescriptor();
}

Cin_Sound::~Cin_Sound(){
    Cin_LoaderFreeData(m_data);
    if(m_event != NULL)
        CloseHandle(m_event);
}
    
bool Cin_Sound::init(){
    // Create the buffer.
    const HRESULT result =
        m_dsound->CreateSoundBuffer(&m_descriptor, &m_buffer, NULL);
    
    if(SUCCEEDED(result)){
        // Fill the buffer.
        write(bufferSize());
        
        if(bufferSize() != m_byte_length){
            IDirectSoundNotify8 *notify;
            const HRESULT query_result =
                m_buffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&notify);
            
            if(FAILED(query_result))
                return false;
            m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
            
            // Set up write notifications
            DSBPOSITIONNOTIFY notes[CIN_DSOUND_NOTIFY_COUNT];
            for(unsigned i = 0; i < CIN_DSOUND_NOTIFY_COUNT; i++){
                notes[i].dwOffset = i * bufferSize() / CIN_DSOUND_NOTIFY_COUNT;
                notes[i].hEventNotify = m_event;
            }
            notify->SetNotificationPositions(CIN_DSOUND_NOTIFY_COUNT, notes);
            
            notify->Release();
        }
        else{
            for(unsigned i = 0; i < CIN_DSOUND_NOTIFY_COUNT; i++){
                m_event = NULL;
            }
        }
        
        return true;
    }
    else{
        return false;
    }
}

void Cin_Sound::onEvent(){
    if(CIN_DSOUND_NOTIFY_COUNT < 3)
        write(bufferSize());
    else
        write((bufferSize() << 1) / CIN_DSOUND_NOTIFY_COUNT);
}

unsigned Cin_StructSoundSize(){
    return sizeof(struct Cin_Sound);
}

enum Cin_LoaderError Cin_LoaderFinailize(struct Cin_Loader *ld,
    struct Cin_Sound *out){
    
    assert(ld->data != NULL);
    CComPtr<IDirectSound8> &dsound_ptr = Cin_GetDriver((Cin_Driver*)ld->data);
    
    new (out) Cin_Sound(dsound_ptr,
        ld->first,
        ld->sample_rate,
        ld->channels,
        ld->format,
        ld->bytes_placed);
    
    if(out->init())
        return Cin_eLoaderSuccess;
    else
        return Cin_eLoaderFailure;
}

void Cin_DestroySound(struct Cin_Sound *snd){
    snd->~Cin_Sound();
}

enum Cin_SoundError Cin_SoundPlay(struct Cin_Sound *snd){
    snd->play();
    return Cin_eSoundSuccess;
}

enum Cin_SoundError Cin_SoundStop(struct Cin_Sound *snd){
    snd->stop();
    return Cin_eSoundSuccess;
}
