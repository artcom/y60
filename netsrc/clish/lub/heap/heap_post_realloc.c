#include <string.h>

#include "private.h"
#include "context.h"
#include "node.h"

lub_heap_status_t
lub_heap_post_realloc(lub_heap_t  *this,
                      char       **ptr,
                      size_t       size)
{
    lub_heap_status_t result = LUB_HEAP_OK;
    
    /* only act if we are about to re-enable the monitoring */
    if((0 < lub_heap_frame_count) && (0 == this->internal))
    {
        lub_heap_leak_mutex_lock();

        if(NULL != *ptr)
        {
            stackframe_t              frame;
            const lub_heap_context_t *context;

            lub_heap__get_stackframe(&frame,lub_heap_frame_count);

            context = lub_heap_context_find_or_create(this,&frame);
 
            if(NULL != context)
            {
                lub_heap_node_t    *node    = (lub_heap_node_t*)*ptr;
                /* initialise the node instance */
                lub_heap_node_init(node,(lub_heap_context_t*)context);

                /* make sure we doctor the pointer given back to the client */
                *ptr = lub_heap_node__get_ptr(node);
            
                lub_heap_leak_stats.allocs++;
                lub_heap_leak_stats.alloc_bytes += lub_heap_node__get_size(node);
                
                /* correct the main heap stats */
                this->stats.alloc_bytes    -= sizeof(lub_heap_node_t);
                this->stats.alloc_overhead += sizeof(lub_heap_node_t);
            }
            else
            {
                unsigned tmp = lub_heap_frame_count;
                /* insufficient resource to allocate a context so fail the alloc */
                lub_heap_frame_count = 0;
                lub_heap_realloc(this,ptr,0,LUB_HEAP_ALIGN_NATIVE);
                *ptr = NULL; /* set this to NULL for the time being */
                result = LUB_HEAP_FAILED;
                lub_heap_frame_count = tmp;
            }
        }
        lub_heap_leak_mutex_unlock();
    }
    return result;
}
