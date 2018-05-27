/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_thread.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static DWORD WINAPI cin_thread_callback(void *a){
    struct Cin_Thread *const thread = a;
    thread->cb(thread->arg);
    ExitThread(0);
}

void Cin_CreateThread(struct Cin_Thread *in_out_thr){
    const HANDLE thread =
        CreateThread(NULL, 0, cin_thread_callback, in_out_thr, 0, NULL);
    in_out_thr->thread = (void*)thread;
}

void Cin_DestroyThread(struct Cin_Thread *thr){
    CloseHandle((HANDLE)thr->thread);
}

void Cin_JoinThread(struct Cin_Thread *thr){
    WaitForSingleObject((HANDLE)thr->thread, INFINITE);
}
