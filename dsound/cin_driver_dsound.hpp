/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_DRIVER_DSOUND_HPP
#define CIN_DRIVER_DSOUND_HPP
#pragma once

#include "mixer/cin_driver_mixer.hpp"

#include <Windows.h>
#include <dsound.h>
#include <objbase.h>
#include <atlbase.h>

struct Cin_Driver : public Cin_DriverMixer {
private:
    
    // Used to signal a callback to fill a buffer.
    // These are 1-indexed for sound, and the 0 object is the signal that there is new data to read.
    std::vector<HANDLE> m_handles;
    std::vector<Cin_Sound*> m_sounds;
    
    CComPtr<IDirectSound8> m_dsound;
    
protected:
    
    virtual void signalNewEvents();
    
    virtual void run();
    
    virtual void remove(const struct Cin_Sound *sound);
    
    virtual void add(struct Cin_Sound *sound, uintptr_t event);
    
public:
    
    inline IDirectSound8 *getDirectSound(){
        return (IDirectSound8*)m_dsound;
    }
    
    Cin_Driver();
    virtual ~Cin_Driver();
};

#endif // CIN_DRIVER_DSOUND_HPP
