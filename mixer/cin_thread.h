/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_THREAD_H
#define CIN_THREAD_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*thread_callback_t)(void *arg);

struct Cin_Thread {
    void *thread, *arg;
    thread_callback_t cb;
};

void Cin_CreateThread(struct Cin_Thread *in_out_thr);

void Cin_DestroyThread(struct Cin_Thread *thr);

void Cin_JoinThread(struct Cin_Thread *thr);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_THREAD_H */
