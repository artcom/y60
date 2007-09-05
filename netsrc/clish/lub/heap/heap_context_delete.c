#include "private.h"
#include "context.h"

void
lub_heap_context_delete(lub_heap_context_t *context)
{
    lub_heap_t *heap = context->heap;
    
    /* finalise the instance */
    lub_heap_context_fini(context);

    /* release the memory */
    heap->internal++;
    lub_dblockpool_free(&heap->context_pool,context);
    heap->internal--;
}
