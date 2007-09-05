#include "private.h"
#include "context.h"

/*--------------------------------------------------------- */
const lub_heap_context_t *
lub_heap_context_find_or_create(lub_heap_t         * this,
                                const stackframe_t * stack)
{
     const lub_heap_context_t *context;
    
    /*
     * look to see whether there is an 
     * existing context for this allocation 
     */
    context = lub_heap_context_find(stack);
    if(NULL == context)
    {
        lub_heap_context_t *new_context;
        this->internal++;
        context = new_context = lub_dblockpool_alloc(&this->context_pool);
        this->internal--;

        if(NULL != context)
        {
            /* initialise the instance */
            lub_heap_context_init(new_context,this,stack);
        }
    }
    return context;
}
/*--------------------------------------------------------- */
