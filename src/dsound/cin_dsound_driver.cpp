// Copyright (c) 2018 AlaskanEmily
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "cin_dsound_driver.hpp"
#include "cin_dsound_sound.hpp"

#include <new>

///////////////////////////////////////////////////////////////////////////////

Cin_Driver::Cin_Driver(){
	CoInitialize(NULL);
    DirectSoundCreate8(NULL, &m_dsound, NULL);
    m_dsound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY);
}

///////////////////////////////////////////////////////////////////////////////

Cin_Driver::~Cin_Driver(){
	m_dsound->Release();
}

///////////////////////////////////////////////////////////////////////////////
	
void Cin_Driver::createSound(Cin_Sound *out, const Cin_Loader &ld){
	new (out) Cin_Sound(m_dsound, ld);
}

///////////////////////////////////////////////////////////////////////////////
// API functions
