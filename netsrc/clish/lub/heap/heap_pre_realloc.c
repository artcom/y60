#include <string.h>

#include "private.h"
#include "context.h"
#include "node.h"

/*
 * This function
 */
void
lub_heap_pre_realloc(lub_heap_t *this,
                     char      **ptr,
                     size_t     *size)
{
    
    if(0 == this->internal)
    {
        lub_heap_leak_mutex_lock();

        if(*ptr)
        {
            /* is this a pointer to a node in the "leak" tree? */
            lub_heap_node_t *node = lub_heap_node_from_ptr(*ptr);

            if(NULL != node)
            {
                if(NULL != lub_heap_node__get_context(node))
                {
                    /* only maintain the leak statistics if this node is being monitored */
                    lub_heap_leak_stats.allocs--;
                    lub_heap_leak_stats.alloc_bytes -= lub_heap_node__get_size(node);
                }
                /* correct the main heap stats */
                this->stats.alloc_bytes    += sizeof(lub_heap_node_t);
                this->stats.alloc_overhead -= sizeof(lub_heap_node_t);
                lub_heap_node_fini(node);

                /* move the pointer to the start of the block */
                *ptr = (char*)node;
            }
        }
        if((0 < lub_heap_frame_count))
        {
            size_t old_size = *size;
            if(old_size)
            {
                /* allocate enough bytes for a node */
                *size += lub_heap_node__get_instanceSize();
                if(*size < old_size)
                {
                    /* we've wrapped the size variable
                     * make sure we fail the allocation
                     */
                    *size = -1;
                }
            }
        }
        lub_heap_leak_mutex_unlock();
    }
}
