/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_select.h"

#include <sys/select.h>

struct cin_pipe {
    int fd[2];
};

struct Cin_SelectSet{
    fd_set set;
    int max_fd;
    unsigned length, capacity;
    struct cin_pipe *pipes;
};

struct Cin_SelectSet *Cin_CreateSelectSet(){
    struct Cin_SelectSet *const set = calloc(sizeof(struct Cin_SelectSet), 1);
    FD_ZERO(&(set->set));
    set->max_fd = -1;
    return set;
}

void Cin_DestroySelectSet(struct Cin_SelectSet *set){
    Cin_ResizeSelectSet(set, 0);
    free(set->pipes);
    free(set);
}

unsigned Cin_SelectSetLength(const struct Cin_SelectSet *set){
    return set->length;
}

void Cin_SwapSelectSetItems(struct Cin_SelectSet *set,
    unsigned a,
    unsigned b){
    
    struct cin_pipe *const a_pipe = set->pipes[a],
        *const b_pipe = set->pipes[b];
    
    int i;
    for(i = 0; i < 2; i++){
        const int f = a_pipe.fd[i];
        a_pipe.fd[i] = b_pipe.fd[i];
        b_pipe.fd[i] = f;
    }
}

void Cin_ResizeSelectSet(struct Cin_SelectSet *set, unsigned newsize){
    unsigned i = set->length, find_new_max = 0, max_fd = set->max_fd;
    struct cin_pipe *const pipes = set->pipes;
    
    if(set->capacity < newsize){
        set->handles = realloc(set->pipes, newsize * sizeof(struct cin_pipe));
        set->capacity = newsize;
    }
    
    while(i > newsize){
        i--;
        {
            const int read_fd = pipes[i].fd[0], write_fd = pipes[i].fd[1];
            close(read_fd);
            close(write_fd);
            FD_CLR(set->set, read_fd);
            if(read_fd == max_fd)
                find_new_max = 1;
        }
    }
    
    while(i < newsize){
        struct cin_pipe *const p = pipes + (i++);
        pipe(p->fd);
        {
            const int read_fd = p->fd[0];
            FD_SET(set->set, read_fd);
            if(read_fd > max_fd){
                max_fd = set->max_fd = read_fd;
            }
        }
    }
    
    if(find_new_max){
        max_fd = -1;
        for(i = 0; i < newsize; i++){
            const int read_fd = set->pipes[i].fd[0];
            if(read_fd > max_fd)
                max_fd = read_fd;
        }
        set->max_fd = max_fd;
    }
    
    set->length = newsize;
}

void Cin_SignalSelectSet(struct Cin_SelectSet *set, unsigned which){
    char c = 0;
    write(set->pipes[which][1], &c, 1);
}

void Cin_Select(struct Cin_SelectSet *set, unsigned *out_which){
    fd_set l_fd_set;
    const int max_fd = set->max_fd + 1;
    
    memcpy(&l_fd_set, &(set->set), sizeof(fd_set));
    select(max_fd, &l_fd_set, NULL, NULL, NULL);
    
    {
        unsigned i;
        for(i = 0; i < max_fd; i++){
            if(FD_ISSET(&l_fd_set, i)){
                char c;
                read(set->pipes[i].fd[0], &c, 1);
                out_which[0] = i;
                return;
            }
        }
    }
    /* ...what? */
    out_which[0] = 0;
}
