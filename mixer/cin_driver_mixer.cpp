/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_driver_mixer.hpp"

Cin_DriverMixer::Cin_DriverMixer()
  : m_dying(false)
  , m_started(false){
    Cin_CreateLock(&m_lock);
}

Cin_DriverMixer::~Cin_DriverMixer(){
    lock();
    if(m_started){
        m_dying = true;
        unlock();
        Cin_JoinThread(&m_thread);
        Cin_DestroyLock(&m_lock);
        Cin_DestroyThread(&m_thread);
    }
    else{
        unlock();
    }
    Cin_DestroyLock(&m_lock);
}

bool Cin_DriverMixer::processEventQueue(){
    
    lock();
    if(m_dying){
        unlock();
        return false;
    }
    
    for(std::vector<event_t>::const_iterator i = m_event_queue.begin();
        i != m_event_queue.end(); i++){
        
        Cin_Sound *const sound = i->second;
        if(const uintptr_t event = i->first){
            add(sound, event);
        }
        else{
            remove(sound);
        }
    }
    
    m_event_queue.clear();
    unlock();
    return true;
}

void Cin_DriverMixer::removeSound(struct Cin_Sound *sound){
    if(sound == NULL)
        return;
    addEvent(0, sound);
}

void Cin_DriverMixer::addSound(struct Cin_Sound *sound, uintptr_t event){
    if(event == 0 || sound == NULL)
        return;
    addEvent(event, sound);
}

void Cin_DriverMixer::start(){
    m_started = true;
    m_thread.arg = this;
    m_thread.cb = ThreadProc;
    Cin_CreateLock(&m_lock);
    Cin_CreateThread(&m_thread);
}

void Cin_DriverMixer::stop(){
    lock();
    m_dying = true;
    unlock();
    Cin_JoinThread(&m_thread);
    Cin_DestroyThread(&m_thread);
}
