/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_LOCK_H
#define CIN_LOCK_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct Cin_Lock {
    void *data;
};

void Cin_CreateLock(struct Cin_Lock *lock);

void Cin_DestroyLock(struct Cin_Lock *lock);

void Cin_ClaimLock(struct Cin_Lock *lock);

void Cin_ReleaseLock(struct Cin_Lock *lock);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CIN_LOCK_H */
