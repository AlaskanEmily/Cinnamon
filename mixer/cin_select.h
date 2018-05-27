/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CIN_SELECT_H
#define CIN_SELECT_H
#pragma once

/* Provides an efficient method to wait on multiple events. */

struct Cin_SelectSet;

struct Cin_SelectSet *Cin_CreateSelectSet();

void Cin_DestroySelectSet(struct Cin_SelectSet *set);

unsigned Cin_SelectSetLength(const struct Cin_SelectSet *set);

void Cin_SwapSelectSetItems(struct Cin_SelectSet *set,
    unsigned a,
    unsigned b);

void Cin_ResizeSelectSet(struct Cin_SelectSet *set, unsigned newsize);

void Cin_SignalSelectSet(struct Cin_SelectSet *set, unsigned which);

void Cin_Select(struct Cin_SelectSet *set, unsigned *out_which);

#endif /* CIN_SELECT_H */
