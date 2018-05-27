/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_thread.h"

#include <pthread.h>
#include <stdlib.h>

static void *cin_thread_wrapper(void *data){
    struct Cin_Thread *const thread = data;
    thread->cb(thread->arg);
    return NULL;
}

void Cin_CreateThread(struct Cin_Thread *in_out_thr){
    pthread_t *const thr = in_out_thr->thread = malloc(sizeof(pthread_t));
    pthread_create(thr, NULL, cin_thread_wrapper, in_out_thr);
}

void Cin_DestroyThread(struct Cin_Thread *thr){
    free(thr->thread);
}

void Cin_JoinThread(struct Cin_Thread *thr){
    void *r;
    pthread_join(thr->thread, &r);
}
