/*
 * heap_destroy.c
 */
#include "private.h"
#include "node.h"
#include "context.h"

/*--------------------------------------------------------- */
static void
lub_heap_node_remove(lub_heap_node_t *node,
                     void            *arg)
{
    lub_heap_t *this = arg;
    lub_heap_context_t *context = lub_heap_node__get_context(node);
    if(context && (context->heap == this))
    {
        lub_heap_leak_stats.allocs--;
        lub_heap_leak_stats.alloc_bytes -= lub_heap_node__get_size(node);
        /* remove this node from the leak tree */
        lub_heap_node_fini(node);
    }
}
/*--------------------------------------------------------- */
void
lub_heap_destroy(lub_heap_t *this)
{
    lub_heap_t **ptr;
    for(ptr = &lub_heap_list;
        *ptr;
        ptr = &(*ptr)->next)
    {
        if((*ptr) == this)
        {
            /* remove from the linked list */
            *ptr = this->next;
            break;
        }
    }

    /* remove all nodes from the leak tree */
    lub_heap_leak_mutex_lock();
    lub_heap_foreach_node(lub_heap_node_remove,this);
    lub_heap_leak_mutex_unlock();
}
/*--------------------------------------------------------- */
