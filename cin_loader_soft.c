#include "cin_loader_soft.h"
#include "cin_loader.h"

#include <stdlib.h>

unsigned Cin_StructLoaderSize(){
    return sizeof(struct Cin_Loader);
}

CIN_PRIVATE(void) Cin_CreateSoftLoader(struct Cin_Loader *out,
    unsigned sample_rate,
    unsigned channels,
    enum Cin_Format format){
    
    out->sample_rate = sample_rate;
    out->channels = channels;
    out->format = format;
    out->bytes_placed = 0;
    out->first = out->last = NULL;
}

enum Cin_LoaderError Cin_LoaderPut(struct Cin_Loader *ld,
    const void *data,
    unsigned byte_size){
    
    struct Cin_LoaderData *const lddata =
        malloc(sizeof(struct Cin_LoaderData) + byte_size);
    
    lddata->next = NULL;
    lddata->len = byte_size;
    
    memcpy(lddata + 1, data, byte_size);
    
    if(ld->first == NULL){
        ld->first = ld->last = lddata;
    }
    else{
        ld->last->next = lddata;
        ld->last = lddata;
    }
    
    ld->bytes_placed += byte_size;
    
    return Cin_eLoaderSuccess;
}

CIN_PRIVATE(void) Cin_LoaderFreeData(struct Cin_LoaderData *data){
    if(data == NULL){
        return;
    }
    else{
        struct Cin_LoaderData *const next = data->next;
        free(data);
        Cin_LoaderFreeData(next);
    }
}

CIN_PRIVATE(void) Cin_LoaderMemcpy(const struct Cin_LoaderData *data,
    unsigned at,
    void *dest,
    unsigned count){
    
    const unsigned len = data->len;
    unsigned char *const dest_bytes = (unsigned char *)dest;
    const struct Cin_LoaderData *const next = data->next;
    
    if(at > len){
        /* Find the first data block with the area we care about. */
        if(next)
            Cin_LoaderMemcpy(next, at - data->len, dest, count);
    }
    else if(at > 0){
        const unsigned remaining = len - at;
        if(remaining >= count){
            /* Copy out the partial area we care about */
            memcpy(dest, data->data + at, count);
        }
        else{
            /* Start our copy from the input index. */
            memcpy(dest, data->data + at, remaining);
            if(next)
                Cin_LoaderMemcpy(next,
                    0,
                    dest_bytes + remaining,
                    count - remaining);
        }
    }
    else{
        /* Copy out the max of our len and the count */
        if(len >= count){
            /* This data holds the remaining count. */
            memcpy(dest, data->data, count);
        }
        else{
            memcpy(dest, data->data, len);
            if(next)
                Cin_LoaderMemcpy(next, 0, dest_bytes + len, count - len);
        }
    }
}
