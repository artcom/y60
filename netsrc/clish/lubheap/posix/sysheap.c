/*
 * sysheap.c
 *
 * This is a replacement of the POSIX memory management system
 * 
 * It uses sbrk() to obtain memory chunks for use by the lub_heap 
 * component
 * 
 * We undefine the public functions
 * (just in case they've been MACRO overriden
 */
#undef calloc
#undef cfree
#undef free
#undef malloc
#undef memalign
#undef realloc
#undef valloc

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>
#define __USE_XOPEN_EXTENDED /* needed for sbrk() */
#include <unistd.h>

#include "lub/heap.h"

#define VX_PAGE_SIZE 4096

typedef struct
{
    pthread_mutex_t mutex;
    lub_heap_t     *heap;
} partition_t;

/* partition used for the system heap */
static partition_t sysMemPartition;

/* the partition is extended in 128K chunks as needed */
#define DEFAULT_CHUNK_SIZE (128 * 1024)

/*-------------------------------------------------------- */
static void
sysheap_fini(void)
{
    /* dump the heap details */
    lub_heap_show(sysMemPartition.heap,1);
}
/*-------------------------------------------------------- */
static void *
sysheap_segment_alloc(size_t required)
{
    return sbrk(required);
}
/*-------------------------------------------------------- */
static void
sysheap_check_memory(size_t required)
{
    partition_t  *this = &sysMemPartition;
    static bool_t initialised,exit_handler;
    void         *segment;
    
    if(BOOL_FALSE == initialised)
    {
        initialised = BOOL_TRUE;
        /* initialise the semaphore for the partition */
        pthread_mutex_init(&this->mutex,NULL);
    
        /* create the heap */
        segment = sysheap_segment_alloc(DEFAULT_CHUNK_SIZE);
        pthread_mutex_lock(&this->mutex);
        this->heap = lub_heap_create(segment,DEFAULT_CHUNK_SIZE);
        pthread_mutex_unlock(&this->mutex);
        
    }
    if((BOOL_FALSE == exit_handler) && lub_heap__get_framecount())
    {
        exit_handler = BOOL_TRUE;
        /*
         * only bother to register an exit handler when leak detection
         * has been switched on
         */
        atexit(sysheap_fini);
    }
    if(lub_heap__get_max_free(this->heap) < required)
    {
        /* we need to expand the heap */
        if(required < DEFAULT_CHUNK_SIZE)
        {
            required = DEFAULT_CHUNK_SIZE;
        }
        pthread_mutex_lock(&this->mutex);
        segment = sysheap_segment_alloc(required);
        lub_heap_add_segment(this->heap,segment,required);
        pthread_mutex_unlock(&this->mutex);
    }
}
/*-------------------------------------------------------- */
static void
sysheap_check_status(partition_t      *this,
                     lub_heap_status_t status,
                     const char       *where,
                     void             *block,
                     size_t            size)
{
    switch(status)
    {
        /*------------------------------------------------- */
        case LUB_HEAP_CORRUPTED:
        {
            fprintf(stderr,"%s: Heap corrupted\n",where);
            break;
        }
        /*------------------------------------------------- */
        case LUB_HEAP_DOUBLE_FREE:
        {
            fprintf(stderr,"%s: Double free of 0x%p\n",where,block);
            break;
        }
        /*------------------------------------------------- */
        case LUB_HEAP_INVALID_POINTER:
        {
            fprintf(stderr,"%s: Invalid Pointer 0x%p\n",where,block);
            break;
        }
        /*------------------------------------------------- */
        case LUB_HEAP_FAILED:
        {
            fprintf(stderr,"%s: allocation of %lu bytes failed\n",
                    where,(unsigned long)size);
            break;
        }
        /*------------------------------------------------- */
        case LUB_HEAP_OK:
        {
            break;
        }
        /*------------------------------------------------- */
    }
}
/*-------------------------------------------------------- */
void *
calloc(size_t nmemb,
       size_t size)
{
    char *ptr = malloc(nmemb*size);
    if(NULL != ptr)
    {
        memset(ptr,0,size);
    }
    return ptr;
}
/*-------------------------------------------------------- */
void
cfree(void *ptr)
{
    free(ptr);
}
/*-------------------------------------------------------- */
void
free(void *ptr)
{
    partition_t      *this   = &sysMemPartition;
    char             *pBlock = ptr;
    lub_heap_status_t status;

    sysheap_check_memory(0);

    pthread_mutex_lock(&this->mutex);
    status = lub_heap_realloc(this->heap,&pBlock,0,LUB_HEAP_ALIGN_NATIVE);
    pthread_mutex_unlock(&this->mutex);

    sysheap_check_status(this,status,"free",pBlock,0);
}
/*-------------------------------------------------------- */
void *
malloc(size_t nBytes)
{
    partition_t      *this = &sysMemPartition;
    char             *ptr  = NULL;
    lub_heap_status_t status;
    
    sysheap_check_memory(nBytes);

    pthread_mutex_lock(&this->mutex);
    status = lub_heap_realloc(this->heap,
                              &ptr,
                              nBytes,
                              LUB_HEAP_ALIGN_NATIVE);
    pthread_mutex_unlock(&this->mutex);

    sysheap_check_status(this,status,"malloc",NULL,nBytes);

    return ptr;
}
/*-------------------------------------------------------- */
void *
memalign(unsigned alignment, 
         unsigned nBytes)
{
    partition_t      *this   = &sysMemPartition;
    char             *pBlock = NULL;
    lub_heap_status_t status;
    lub_heap_align_t  align;

    switch(alignment)
    {
        case 4:
            align = LUB_HEAP_ALIGN_4;
            break;
        case 8:
            align = LUB_HEAP_ALIGN_8;
            break;
        case 16:
            align = LUB_HEAP_ALIGN_16;
            break;
        case 32:
            align = LUB_HEAP_ALIGN_32;
            break;
        case 64:
            align = LUB_HEAP_ALIGN_64;
            break;
        case 128:
            align = LUB_HEAP_ALIGN_128;
            break;
        case 256:
            align = LUB_HEAP_ALIGN_256;
            break;
        case 512:
            align = LUB_HEAP_ALIGN_512;
            break;
        case 1024:
            align = LUB_HEAP_ALIGN_1024;
            break;
        case 2048:
            align = LUB_HEAP_ALIGN_2048;
            break;
        case 4096:
            align = LUB_HEAP_ALIGN_4096;
            break;
        case 8192:
            align = LUB_HEAP_ALIGN_8192;
            break;
        case 16384:
            align = LUB_HEAP_ALIGN_16384;
            break;
        case 32768:
            align = LUB_HEAP_ALIGN_32768;
            break;
        default:
            align = LUB_HEAP_ALIGN_NATIVE;
            break;
    }
    pthread_mutex_lock(&this->mutex);
    status = lub_heap_realloc(this->heap,
                              &pBlock,
                              nBytes,
                              align);
    pthread_mutex_unlock(&this->mutex);

    sysheap_check_status(this,status,"memalign",pBlock,nBytes);

    return (LUB_HEAP_OK == status) ? pBlock : NULL;
}
/*-------------------------------------------------------- */
void *
realloc(void   *old_ptr, 
        size_t  nBytes)
{
    partition_t      *this   = &sysMemPartition;
    char             *pBlock = old_ptr;
    lub_heap_status_t status;

    sysheap_check_memory(nBytes);

    pthread_mutex_lock(&this->mutex);
    status = lub_heap_realloc(this->heap,
                              &pBlock,
                              nBytes,
                              LUB_HEAP_ALIGN_NATIVE);
    pthread_mutex_unlock(&this->mutex);

    sysheap_check_status(this,status,"realloc",pBlock,nBytes);

    return (LUB_HEAP_OK == status) ? pBlock : NULL;
}
/*-------------------------------------------------------- */
void *
valloc(unsigned size)
{
    return memalign(VX_PAGE_SIZE,size);
}
/*-------------------------------------------------------- */
