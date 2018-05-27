/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_select.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct Cin_SelectSet{
    unsigned length, capacity;
    HANDLE *handles;
};

struct Cin_SelectSet *Cin_CreateSelectSet(){
    return calloc(sizeof(struct Cin_SelectSet), 1);
}

void Cin_DestroySelectSet(struct Cin_SelectSet *set){
    Cin_ResizeSelectSet(set, 0);
    free(set->handles);
    free(set);
}

unsigned Cin_SelectSetLength(const struct Cin_SelectSet *set){
    return set->length;
}

void Cin_SwapSelectSetItems(struct Cin_SelectSet *set,
    unsigned a,
    unsigned b){
    
    const HANDLE tmp = set->handles[a];
    set->handles[a] = set->handles[b];
    set->handles[b] = tmp;
}

void Cin_ResizeSelectSet(struct Cin_SelectSet *set, unsigned newsize){
    unsigned i = set->length;
    HANDLE *const handles = set->handles;
    
    if(set->capacity < newsize){
        set->handles = realloc(set->handles, newsize * sizeof(HANDLE));
        set->capacity = newsize;
    }
    
    while(i > newsize){
        CloseHandle(handles[--i]);
    }
    while(i < newsize){
        handles[i++] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
    set->length = newsize;
}

void Cin_SignalSelectSet(struct Cin_SelectSet *set, unsigned which){
    SetEvent(set->handles[which]);
}

void Cin_Select(struct Cin_SelectSet *set, unsigned *out_which){
    const unsigned length = set->length;
    const DWORD value =
        WaitForMultipleObjects(length, set->handles, FALSE, INFINITE);
    const unsigned which = value - WAIT_OBJECT_0;
    out_which[0] = (which < length) ? which : 0;
}
