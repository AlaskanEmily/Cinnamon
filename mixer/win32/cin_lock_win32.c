/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_lock.h"

#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0400

#include <Windows.h>
#include <stdlib.h>

void Cin_CreateLock(struct Cin_Lock *lock){
    lock->data = malloc(sizeof(CRITICAL_SECTION));
    InitializeCriticalSection(lock->data);
}

void Cin_DestroyLock(struct Cin_Lock *lock){
    DeleteCriticalSection(lock->data);
    free(lock->data);
}

void Cin_ClaimLock(struct Cin_Lock *lock){
    EnterCriticalSection(lock->data);
}

void Cin_ReleaseLock(struct Cin_Lock *lock){
    LeaveCriticalSection(lock->data);
}
