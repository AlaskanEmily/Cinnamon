/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_DRIVER_MIXER_HPP
#define CIN_DRIVER_MIXER_HPP
#pragma once

#include "cin_lock.h"
#include "cin_thread.h"

#include <stdint.h>
#include <vector>

struct Cin_Sound;

class Cin_DriverMixer {
    
protected:
    
    Cin_DriverMixer();
    
public:
    virtual ~Cin_DriverMixer();
    
private:
    typedef std::pair<uintptr_t, Cin_Sound *> event_t;
    std::vector<event_t> m_event_queue;
    Cin_Lock m_lock;
    Cin_Thread m_thread;
    bool m_dying, m_started;
    
protected:
    
    // The lock will be claimed while this is called.
    virtual void signalNewEvents() = 0;
    
    virtual void run() = 0;
    
    virtual void remove(const struct Cin_Sound *sound) = 0;
    
    virtual void add(struct Cin_Sound *sound, uintptr_t event) = 0;
    
    bool processEventQueue();

private:
    
    static void ThreadProc(void *that){
        static_cast<Cin_DriverMixer*>(that)->run();
    }
    
public:
    
    // These should only be used by the backend when modifying the underlying
    // sound objects. The interface for the mixer (run, add, remove, etc)
    // should NOT call these methods. Additionally, the addSound/removeSound
    // methods call these methods, and so the caller should not explicitly
    // lock/unlock the driver for those methods.
    inline void lock(){ Cin_ClaimLock(&m_lock); }
    inline void unlock(){ Cin_ReleaseLock(&m_lock); }
    
    void removeSound(struct Cin_Sound *sound);
    void addSound(struct Cin_Sound *sound, uintptr_t event);
    void start();
    void stop();
    
private:

    inline void addEvent(uintptr_t event, Cin_Sound *sound){
        lock();
        m_event_queue.push_back(event_t(event, sound));
        signalNewEvents();
        unlock();
    }
};

#endif // CIN_DRIVER_MIXER_HPP
