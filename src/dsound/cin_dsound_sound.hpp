// Copyright (c) 2018 AlaskanEmily
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef CIN_DSOUND_SOUND_HPP
#define CIN_DSOUND_SOUND_HPP
#pragma once

///////////////////////////////////////////////////////////////////////////////

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <objbase.h>

///////////////////////////////////////////////////////////////////////////////

struct Cin_Loader;

///////////////////////////////////////////////////////////////////////////////

struct Cin_Sound{
private:
    IDirectSoundBuffer *m_buffer;
    IDirectSound8 *const m_dsound;

public:

    Cin_Sound(IDirectSound8 *dsound, const Cin_Loader &ld);
    ~Cin_Sound();

    void play(bool loop);
    void stop();
};

///////////////////////////////////////////////////////////////////////////////

#endif // CIN_DSOUND_SOUND_HPP
