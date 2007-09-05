/*
 * node.c
 */
#include <string.h>
#include <assert.h>

#include "private.h"
#include "context.h"
#include "node.h"

/* 
 * This tree holds all the nodes allocated in the system
 * irrespective of the heap to which they belong
 */
lub_bintree_t node_tree;

/*--------------------------------------------------------- */
int
lub_heap_node_compare(const void *clientnode,
                      const void *clientkey)
{
    const lub_heap_node_t     * node = clientnode;
    const lub_heap_node_key_t * key  = clientkey;
    
    return ((long)node - (long)key->node);
}
/*--------------------------------------------------------- */
/* we simply use the node address as the index */
void
lub_heap_node_getkey(const void        *clientnode,
                    lub_bintree_key_t *key)
{
    lub_heap_node_key_t clientkey;
    clientkey.node = clientnode;
    memcpy(key,&clientkey,sizeof(lub_heap_node_key_t));
}
/*--------------------------------------------------------- */
static void
lub_heap_node_meta_init(void)
{
    static bool_t initialised = BOOL_FALSE;
    
    if(BOOL_FALSE == initialised)
    {
        initialised = BOOL_TRUE;
        /* initialise the node tree */
        lub_bintree_init(&node_tree,
                         offsetof(lub_heap_node_t,bt_node),
                         lub_heap_node_compare,
                         lub_heap_node_getkey);
    }
}
/*--------------------------------------------------------- */
void
lub_heap_node_init(lub_heap_node_t    * this,
                   lub_heap_context_t * context)
{
    lub_heap_node_meta_init();
    
    /* initialise the control blocks */
    lub_bintree_node_init(&this->bt_node);

    lub_heap_node__set_context(this,context);
    lub_heap_node__set_leaked(this,BOOL_FALSE);
    lub_heap_node__set_partial(this,BOOL_FALSE);
    lub_heap_node__set_next(this,NULL);
    lub_heap_node__set_scanned(this,BOOL_FALSE);
    this->prev = NULL;
    
    /* add this to the node tree */
    lub_bintree_insert(&node_tree,this);

    /* add ourselves to this context */
    lub_heap_context_insert_node(context,this);
}
/*--------------------------------------------------------- */
void
lub_heap_node_fini(lub_heap_node_t * this)
{
    /* remove this from the node tree */
    lub_bintree_remove(&node_tree,this);

    /* remove this node from this context */
    lub_heap_node_clear(this);
}
/*--------------------------------------------------------- */
size_t
    lub_heap_node__get_instanceSize(void)
{
    return sizeof(lub_heap_node_t);
}
/*--------------------------------------------------------- */
size_t
    lub_heap_node__get_size(const lub_heap_node_t *this)
{
    return lub_heap__get_block_size(this) - sizeof(lub_heap_node_t);
}
/*--------------------------------------------------------- */
char *
    lub_heap_node__get_ptr(lub_heap_node_t *this)
{
    return (char*)&this[1];
}
/*--------------------------------------------------------- */
bool_t 
lub_heap_validate_pointer(lub_heap_t *this,
                          char       *ptr)
{
    bool_t result = BOOL_FALSE;
    do
    {
        lub_heap_segment_t *seg;

        if((0 < lub_heap_frame_count) && (0 == this->internal))
        {
            /*
             * When leak detection is enabled we can perform detailed 
             * validation of pointers.
             */
            lub_heap_node_t *node = lub_heap_node_from_ptr(ptr);
            if(NULL != node)
            {
                lub_heap_context_t *context = lub_heap_node__get_context(node);
                if(NULL != context)
                {
                    /* we've found a context so can give a definative answer */
                    if(this == context->heap)
                    {
                        result = BOOL_TRUE;
                    }
                    break;
                }
            }
        }
    
        /* iterate around each of the segments belonging to this heap */
        for(seg = &this->first_segment;
            seg;
            seg = seg->next)
        {
            char *start = (char*)&seg[1];
            if(ptr >= start)
            {
                char *end = start + (seg->words << 2);
                if(ptr < end)
                {
                    /* we've found a parent segment for this pointer */
                    result = BOOL_TRUE;
                    break;
                }
            }
        }
    } while(0);

    return result;
}
/*--------------------------------------------------------- */
lub_heap_node_t *
lub_heap_node_from_ptr(char *ptr)
{
    lub_heap_node_t    *result;
    lub_heap_node_key_t key;
        
    /* search for the node which comes immediately before this pointer */
    key.node = (lub_heap_node_t *)ptr;
    result = lub_bintree_findprevious(&node_tree,&key);
    
    if((NULL != result))
    {
        char *tmp = lub_heap_node__get_ptr(result);
        /* ensure that the pointer is within the scope of this node */
        if(ptr < tmp)
        {
            result = NULL;
        }
        else
        {
            /* compare with the end of the allocated memory */
            tmp += lub_heap__get_block_size(result);
            /* ensure that the pointer is within the scope of this node */
            if(ptr >= tmp)
            {
                /* out of range of this node */
                result = NULL;
            }
        }
    }
    return result;
}
/*--------------------------------------------------------- */
void
lub_heap_node__set_context(lub_heap_node_t    *this,
                        lub_heap_context_t *value)
{
    unsigned long mask = ((unsigned long)value & ~(LEAKED_MASK | PARTIAL_MASK));
    this->_context = (lub_heap_context_t *)mask;
}
/*--------------------------------------------------------- */
lub_heap_context_t *
lub_heap_node__get_context(const lub_heap_node_t *this)
{
    unsigned long mask = (unsigned long)this->_context & ~(LEAKED_MASK | PARTIAL_MASK);
    return (lub_heap_context_t *)mask;
}
/*--------------------------------------------------------- */
void
lub_heap_node__set_next(lub_heap_node_t *this,
                     lub_heap_node_t *value)
{
    unsigned long mask = ((unsigned long)value & ~(SCANNED_MASK));
    this->_next = (lub_heap_node_t *)mask;
}
/*--------------------------------------------------------- */
lub_heap_node_t *
lub_heap_node__get_next(const lub_heap_node_t *this)
{
    unsigned long mask = (unsigned long)this->_next & ~(SCANNED_MASK);
    return (lub_heap_node_t *)mask;
}
/*--------------------------------------------------------- */
bool_t
lub_heap_node__get_leaked(const lub_heap_node_t *this)
{
    return ((unsigned long)this->_context & LEAKED_MASK) ? BOOL_TRUE : BOOL_FALSE;
}
/*--------------------------------------------------------- */
bool_t
lub_heap_node__get_partial(const lub_heap_node_t *this)
{
    return ((unsigned long)this->_context & PARTIAL_MASK) ? BOOL_TRUE : BOOL_FALSE;
}
/*--------------------------------------------------------- */
bool_t
lub_heap_node__get_scanned(const lub_heap_node_t *this)
{
    return ((unsigned long)this->_next & SCANNED_MASK) ? BOOL_TRUE : BOOL_FALSE;
}
/*--------------------------------------------------------- */
void
lub_heap_node__set_leaked(lub_heap_node_t *this, bool_t value)
{
    unsigned long mask = (unsigned long)this->_context;
    if(BOOL_TRUE == value)
    {
        mask |= LEAKED_MASK;
    }
    else
    {
        mask &= ~LEAKED_MASK;
    }
    this->_context = (lub_heap_context_t *)mask;
}
/*--------------------------------------------------------- */
void
lub_heap_node__set_partial(lub_heap_node_t *this, bool_t value)
{
    unsigned long mask = (unsigned long)this->_context;
    if(BOOL_TRUE == value)
    {
        mask |= PARTIAL_MASK;
    }
    else
    {
        mask &= ~PARTIAL_MASK;
    }
    this->_context = (lub_heap_context_t *)mask;
}
/*--------------------------------------------------------- */
void
lub_heap_node__set_scanned(lub_heap_node_t *this, bool_t value)
{
    unsigned long mask = (unsigned long)this->_next;
    if(BOOL_TRUE == value)
    {
        mask |= SCANNED_MASK;
    }
    else
    {
        mask &= ~SCANNED_MASK;
    }
    this->_next = (lub_heap_node_t *)mask;
}
/*--------------------------------------------------------- */
void
lub_heap_foreach_node(void (*fn)(lub_heap_node_t *,void*),
                      void  *arg)
{
    lub_heap_node_t    * node;
    lub_heap_node_key_t  key;

    for(node = lub_bintree_findfirst(&node_tree);
        node;
        node = lub_bintree_findnext(&node_tree,&key))
    {
        lub_heap_node_getkey(node,(lub_bintree_key_t*)&key);
        /* invoke the specified method on this node */
        fn(node,arg);
    }
}
/*--------------------------------------------------------- */
void
lub_heap_node_clear(lub_heap_node_t *this)
{
    /* clearing a node removes it from it's context */
    lub_heap_context_t *context = lub_heap_node__get_context(this);
    if(NULL != context)
    {
        lub_heap_node__set_context(this,NULL);
        lub_heap_node__set_leaked(this,BOOL_FALSE);
        lub_heap_node__set_partial(this,BOOL_FALSE);
        lub_heap_context_remove_node(context,this);
    }
}
/*--------------------------------------------------------- */
void 
lub_heap_scan_memory(const void  *mem,
                     size_t       size)
{
    size_t      bytes_left = size;
    typedef void *void_ptr;
    const void_ptr *ptr;

    lub_heap_leak_stats.scanned++;
    
    /* scan all the words in this allocated block of memory */
    for(ptr = mem;
        bytes_left;
        )
    {
        lub_heap_node_t    *node;
        lub_heap_node_key_t key;
        
        /* search for a node */
        key.node = (lub_heap_node_t *)ptr;
        node = lub_bintree_find(&node_tree,&key);
        if(NULL != node)
        {
            /*
             * If we stumble across a node don't scan it's contents as this could cause
             * false negatives. This situation could occur if an allocated block of memory
             * was used as a heap in it's own right.
             */
            char  *tmp       = lub_heap_node__get_ptr(node);
            size_t node_size = lub_heap_node__get_size(node);
            
            /* skip forward past the node contents */
            ptr         = (void**)(tmp + node_size);
            bytes_left -= (node_size + sizeof(lub_heap_node_t));
            continue;
        }
        /* 
         * see whether this is a reference to a node 
         * NB. we only resolve a node if the pointer lies
         * within the memory allocated to the client; any direct
         * references to nodes will not resolve to a node
         * object.
         */
        node = lub_heap_node_from_ptr(*ptr);
        if( (NULL != node))
        {
            /* this looks like a valid node */
            lub_heap_context_t *context   = lub_heap_node__get_context(node);
            size_t              node_size = lub_heap_node__get_size(node);
            char               *tmp       = lub_heap_node__get_ptr(node);
            if(tmp == *ptr)
            {
                /* reference to start of block */
                if(BOOL_TRUE == lub_heap_node__get_partial(node))
                {
                    lub_heap_node__set_partial(node,BOOL_FALSE);

                    if(NULL != context)
                    {
                        context->partials--;
                        context->partial_bytes -= node_size;

                        lub_heap_leak_stats.partials--;
                        lub_heap_leak_stats.partial_bytes -= node_size;
                    }
                }
            }
            /* this is definately not a leak */
            if(BOOL_TRUE == lub_heap_node__get_leaked(node))
            {
                lub_heap_node__set_leaked(node,BOOL_FALSE);

                if(NULL != context)
                {
                    context->leaks--;
                    context->leaked_bytes -= node_size;
 
                    lub_heap_leak_stats.leaks--;
                    lub_heap_leak_stats.leaked_bytes -= node_size;
                }
            }
        }
        ptr++;
        bytes_left -= sizeof(void*);
    }
}
/*--------------------------------------------------------- */
