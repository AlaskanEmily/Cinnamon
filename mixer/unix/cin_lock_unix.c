/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_lock.h"

#include <pthread.h>
#include <stdlib.h>

void Cin_CreateLock(struct Cin_Lock *lock){
    void *mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);
    lock->data = mutex;
}

void Cin_DestroyLock(struct Cin_Lock *lock){
    pthread_mutex_destroy(lock->data);
    free(lock->data);
}

void Cin_ClaimLock(struct Cin_Lock *lock){
    pthread_mutex_lock(lock->data);
}

void Cin_ReleaseLock(struct Cin_Lock *lock){
    pthread_mutex_unlock(lock->data);
}
