// Copyright (c) 2018 AlaskanEmily
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef CIN_DSOUND_HPP
#define CIN_DSOUND_HPP
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

struct Cin_Sound;
struct Cin_Loader;

///////////////////////////////////////////////////////////////////////////////

struct Cin_Driver {
private:
	IDirectSound8 *m_dsound;
public:
	
	Cin_Driver();
	~Cin_Driver();
	
	void createSound(Cin_Sound *out, const Cin_Loader &ld);
	
};

///////////////////////////////////////////////////////////////////////////////

#endif // CIN_DSOUND_HPP
