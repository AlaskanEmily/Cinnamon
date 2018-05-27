/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_DRIVER_OSS_HPP
#define CIN_DRIVER_OSS_HPP
#pragma once

#include "mixer/cin_driver_mixer.hpp"

#include "cin_sound_oss.h"

struct Cin_Driver : public Cin_DriverMixer {
private:

protected:
    
    virtual void signalNewEvents();
    
    virtual void run();
    
    virtual void remove(const struct Cin_Sound *sound);
    
    virtual void add(struct Cin_Sound *sound, uintptr_t event);
    
public:
    
};

#endif // CIN_DRIVER_OSS_HPP
