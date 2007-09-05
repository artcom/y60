#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "private.h"
#include "context.h"
#include "node.h"

static lub_bintree_t         context_tree;

lub_heap_leak_stats_t lub_heap_leak_stats;
unsigned long         lub_heap_frame_count = 0;

/*--------------------------------------------------------- 
 * PRIVATE META FUNCTIONS
 *--------------------------------------------------------- */
void
lub_heap_context_meta_init(void)
{
    static bool_t initialised = BOOL_FALSE;
    if(BOOL_FALSE == initialised)
    {
        initialised = BOOL_TRUE;

        /* initialise the context tree */
        lub_bintree_init(&context_tree,
                         offsetof(lub_heap_context_t,bt_node),
                         lub_heap_context_compare,
                         lub_heap_context_getkey);
    }
}
/*--------------------------------------------------------- */
int
lub_heap_context_compare(const void *clientnode,
                         const void *clientkey)
{
    int                           i,delta=0;
    const lub_heap_context_t     *node = clientnode;
    const lub_heap_context_key_t *key  = clientkey;    
    function_t                  **node_fn = node->key.backtrace;
    function_t                  **key_fn  = key->backtrace;
        
    for(i = lub_heap_frame_count;
        i;
        i--,node_fn++,key_fn++)
    {
        delta = ((unsigned long)*node_fn - (unsigned long)*key_fn);
        if(0 != delta)
        {
            break;
        }
    }
    return delta;
}
/*--------------------------------------------------------- */
/* we simply use the embedded key as the index */
void
lub_heap_context_getkey(const void        *clientnode,
                        lub_bintree_key_t *key)
{
    const lub_heap_context_t * context = clientnode;
    memcpy(key,&context->key,sizeof(lub_heap_context_key_t));
}
/*--------------------------------------------------------- */
static void
lub_heap_foreach_context(void (*fn)(lub_heap_context_t *))
{
    lub_heap_context_t    * context;
    lub_bintree_iterator_t  iter;

    for(context = lub_bintree_findfirst(&context_tree),
        context ? lub_bintree_iterator_init(&iter,&context_tree,context) : (void)0;
        context;
        context = lub_bintree_iterator_next(&iter))
    {
        /* invoke the specified method on this context */
        fn(context);
    }
}
/*--------------------------------------------------------- */
static void
lub_heap_show_summary(void)
{
    size_t ok_allocs = lub_heap_leak_stats.allocs;
    size_t ok_bytes  = lub_heap_leak_stats.alloc_bytes; 
    
    ok_allocs -= lub_heap_leak_stats.partials;
    ok_allocs -= lub_heap_leak_stats.leaks;
    
    ok_bytes -= lub_heap_leak_stats.partial_bytes;
    ok_bytes -= lub_heap_leak_stats.leaked_bytes;
    
    printf("\n"
           "          +----------+----------+----------+----------+\n");
    printf("  TOTALS  |    blocks|     bytes|   average|  overhead|\n");
    printf("+---------+----------+----------+----------+----------+\n");
    printf("|contexts |%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|\n",
            lub_heap_leak_stats.contexts,
            lub_heap_leak_stats.contexts * sizeof(lub_heap_context_t),
            lub_heap_leak_stats.contexts ? sizeof(lub_heap_context_t) : 0,
            lub_heap_leak_stats.contexts * lub_heap__get_block_overhead());
    printf("|allocs   |%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|\n",
            ok_allocs,
            ok_bytes,
            ok_allocs ? (ok_bytes / ok_allocs) : 0,
            ok_allocs*(lub_heap__get_block_overhead() + sizeof(lub_heap_node_t)));
    printf("|partials |%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|\n",
            lub_heap_leak_stats.partials,
            lub_heap_leak_stats.partial_bytes,
            lub_heap_leak_stats.partials ? (lub_heap_leak_stats.partial_bytes / lub_heap_leak_stats.partials) : 0,
            lub_heap_leak_stats.partials*(lub_heap__get_block_overhead() + sizeof(lub_heap_node_t)));
    printf("|leaks    |%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|\n",
            lub_heap_leak_stats.leaks,
            lub_heap_leak_stats.leaked_bytes,
            lub_heap_leak_stats.leaks ? (lub_heap_leak_stats.leaked_bytes / lub_heap_leak_stats.leaks) : 0,
            lub_heap_leak_stats.leaks*(lub_heap__get_block_overhead() + sizeof(lub_heap_node_t)));
    printf("+---------+----------+----------+----------+----------+\n");
}
/*--------------------------------------------------------- */

/*--------------------------------------------------------- 
 * PRIVATE METHODS
 *--------------------------------------------------------- */
static void
lub_heap_context_foreach_node(lub_heap_context_t *this,
                              void           (*fn)(lub_heap_node_t *))
{
    lub_heap_node_t * node;
 
    for(node = this->first_node;
        node;
        node = lub_heap_node__get_next(node))
    {
        /* invoke the specified method on this node */
        fn(node);
    }
}
/*--------------------------------------------------------- */

/*--------------------------------------------------------- 
 * PUBLIC METHODS
 *--------------------------------------------------------- */
void
lub_heap_context_init(lub_heap_context_t * this,
                      lub_heap_t         * heap,
                      const stackframe_t * stack)
{
    lub_heap_context_meta_init();
    
    this->heap          = heap;
    this->allocs        = 0;
    this->alloc_bytes   = 0;
    this->leaks         = 0;
    this->leaked_bytes  = 0;
    this->partials      = 0;
    this->partial_bytes = 0;
    this->first_node    = NULL;
    
    /* set the current backtrace for the context */
    this->key = *stack;
    
    /* intialise this context's binary tree node */
    lub_bintree_node_init(&this->bt_node);

    /* add this context to the context_tree */
    lub_bintree_insert(&context_tree,this);
    
    lub_heap_leak_stats.contexts++;
}
/*--------------------------------------------------------- */
void
lub_heap_context_fini(lub_heap_context_t * this)
{
    /* remove this node from the context_tree */
    lub_bintree_remove(&context_tree,this);
    
    /* cleanup the context */
    lub_heap_context_clear(this);

    lub_heap_leak_stats.contexts--;
}
/*--------------------------------------------------------- */
void
lub_heap_context_insert_node(lub_heap_context_t * this,
                             lub_heap_node_t    * node)
{
    /* add the node to the linked list */
    lub_heap_node__set_next(node,this->first_node);
    node->prev = NULL;
    if(this->first_node)
    {
        this->first_node->prev = node;
    }
    this->first_node = node;
    
    /* increment the allocation count */
    this->allocs++;
    this->alloc_bytes += lub_heap_node__get_size(node);
}
/*--------------------------------------------------------- */
void
lub_heap_context_remove_node(lub_heap_context_t *this,
                             lub_heap_node_t    *node)
{
    lub_heap_node_t *next,*prev = NULL;
    
    /* remove the node from the context list */
    next = lub_heap_node__get_next(node);
    prev = node->prev;

    if(NULL == prev)
    {
        this->first_node = next;
    }
    else
    {
        lub_heap_node__set_next(prev,next);
    }
    if(NULL != next)
    {
        next->prev = prev;
    }

    /* clear the pointers */
    lub_heap_node__set_next(node,NULL);
    node->prev = NULL;
    
    /* increment the free count */
    this->allocs--;
    this->alloc_bytes -= lub_heap_node__get_size(node);

    /* is this the last node in a context? */
    if(0 == this->allocs)
    {
        /* removing the last node deletes the context */
        lub_heap_context_delete(this);
    }
}
/*--------------------------------------------------------- */
void
lub_heap_context_show_frame(lub_heap_context_t * this,
                            int                  frame)
{
    if(frame >= 0)
    {
        long address = (long)this->key.backtrace[frame]; 
        if(address)
        {
            lub_heap_symShow(address);
        }
    }
    printf("\n");
}
/*--------------------------------------------------------- */
void
lub_heap_node_post(lub_heap_node_t *node)
{
    /* assume the worst */
    if(BOOL_TRUE == lub_heap_node__get_leaked(node))
    {
        /* this is a full leak */
        lub_heap_node__set_partial(node,BOOL_FALSE);
    }
}
/*--------------------------------------------------------- */
void
lub_heap_node_prep(lub_heap_node_t *node,
                   void            *arg)
{
    /* assume the worst */
    lub_heap_node__set_leaked(node,BOOL_TRUE);
    lub_heap_node__set_partial(node,BOOL_TRUE);
    lub_heap_node__set_scanned(node,BOOL_FALSE);
}
/*--------------------------------------------------------- */
void
lub_heap_node_scan(lub_heap_node_t *node,
                   void            *arg)
{
    /* only scan nodes which have references */
    if(   (BOOL_FALSE == lub_heap_node__get_leaked(node) )
       && (BOOL_FALSE == lub_heap_node__get_scanned(node)) )
    {
        lub_heap_node__set_scanned(node,BOOL_TRUE);
        lub_heap_scan_memory(lub_heap_node__get_ptr(node),
                             lub_heap__get_block_size(node) - lub_heap_node__get_instanceSize());
    }
}
/*--------------------------------------------------------- */
void
lub_heap_context_prep(lub_heap_context_t *this)
{
    /* start off by assuming the worst */
    this->partials      = this->leaks        = this->allocs;
    this->partial_bytes = this->leaked_bytes = this->alloc_bytes;
}
/*--------------------------------------------------------- */
void
lub_heap_context_post(lub_heap_context_t *this)
{
    /* don't count full leaks as partials */
    this->partials      -= this->leaks;
    this->partial_bytes -= this->leaked_bytes;
    
    /* post process the contained nodes */
    lub_heap_context_foreach_node(this,lub_heap_node_post);
}
/*--------------------------------------------------------- */
static void
scan_segment(void    *segment,
             unsigned index,
             size_t   size,
             void    *dummy)
{
    lub_heap_block_t *block;
    
    /* now iterate along the block chain checking each one */
    for(block = lub_heap_block_getfirst(segment);
        block;
        block = lub_heap_block_getnext(block))
    {
        if(BOOL_FALSE == block->free.tag.free)
        {
            /* is this allocated block in the leak tree? */
            lub_heap_node_t *node = lub_heap_node_from_ptr((void*)block->alloc.memory);
            if(NULL == node)
            {
                /* this is a non-monitored block */
                const lub_heap_segment_t *segment;
                const char               *ptr = (void*)block->alloc.memory;
                size_t                    len = (block->alloc.tag.words << 2) - sizeof(lub_heap_alloc_block_t);
                segment = lub_heap_segment_findnext(ptr);
                if((NULL == segment) || ((long)(ptr + len) < (long)segment))
                {
                    /* OK this block doesn't contain another segment */
                    lub_heap_scan_memory(ptr,len);
                }
            }
        }
    }
}
/*--------------------------------------------------------- */
/** 
 * This function scans all the nodes currently allocated in the
 * system for references to other allocated nodes.
 * First of all we mark all nodes as leaked, then scan all the nodes
 * for any references to other ones. If found those other ones 
 * are cleared from being leaked.
 * At the end of the process all nodes which are leaked then
 * update their context leak count.
 */
void
lub_heap_scan_all(void)
{
    lub_heap_t *heap;
    
    printf("  Scanning memory");

    /* assume the worst */
    lub_heap_leak_stats.partials      = lub_heap_leak_stats.leaks        = lub_heap_leak_stats.allocs;
    lub_heap_leak_stats.partial_bytes = lub_heap_leak_stats.leaked_bytes = lub_heap_leak_stats.alloc_bytes;
    
    /* first of all prepare the contexts for scanning */
    lub_heap_foreach_context(lub_heap_context_prep);

    /* then prepare all the nodes (including those who have no context) */
    lub_heap_foreach_node(lub_heap_node_prep,NULL);
    
    /* clear out the stacks in the system */
    lub_heap_clean_stacks();

    /* Scan the current stack */
    printf(".");
    lub_heap_scan_stack();

    /* Scan the BSS segment */
    printf(".");
    lub_heap_scan_bss();

    /* Scan the DATA segment */
    printf(".");
    lub_heap_scan_data();

    /* Scan the non-monitored blocks which are allocated in the system */
    for(heap = lub_heap_list;
        heap;
        heap = heap->next) 
    {
        printf(".");
        lub_heap_foreach_segment(heap,scan_segment,NULL);
    }

    /* 
     * now scan the nodes NB. only referenced nodes will be scanned 
     * we loop until we stop scanning new nodes
     */
    do
    {
        lub_heap_leak_stats.scanned = 0;
        printf(".");
        /* scan each node */
        lub_heap_foreach_node(lub_heap_node_scan,NULL);
    } while(lub_heap_leak_stats.scanned);
    
    /* post process each context and contained nodes */
    lub_heap_foreach_context(lub_heap_context_post);
    
    /* don't count the no-context node statistics */
    lub_heap_leak_stats.partials      -= lub_heap_leak_stats.leaks;
    lub_heap_leak_stats.partial_bytes -= lub_heap_leak_stats.leaked_bytes;
    
    printf("done\n\n");
}
/*--------------------------------------------------------- */
void lub_heap_node_show(lub_heap_node_t *node)
{
    printf("%s%p[%"SIZE_FMT"] ",
           lub_heap_node__get_leaked(node) ? "*" : lub_heap_node__get_partial(node) ? "+" : "",
           lub_heap_node__get_ptr(node),
           lub_heap__get_block_size(node) - sizeof(lub_heap_node_t));
}
/*--------------------------------------------------------- */
void
lub_heap_context_show(lub_heap_context_t *this,
                      lub_heap_show_e     how)
{
    long frame = lub_heap_frame_count-1;
    
    size_t ok_allocs = this->allocs;
    size_t ok_bytes  = this->alloc_bytes;
    
    ok_allocs -= this->partials;
    ok_allocs -= this->leaks;

    ok_bytes  -= this->partial_bytes;
    ok_bytes  -= this->leaked_bytes;

    switch(how)
    {
        case LUB_HEAP_SHOW_ALL:
        {
            /* show everything */
            break;
        }
        case LUB_HEAP_SHOW_PARTIALS:
        {
            if(0 < this->partials)
            {
                /* there's at least one partial in this context */
                break;
            }
            /* fall through */
        }
        case LUB_HEAP_SHOW_LEAKS:
        {
            if(0 < this->leaks)
            {
                /* there's at least one leak in this context */
                break;
            }
            /* fall through */
        }
        default:
        {
            /* nothing to be shown */
            return;
        }
    }

    /* find the top of the stack trace */
    while((frame >= 0) && (0 == this->key.backtrace[frame]))
    {
        frame--;
    }
    printf("          +----------+----------+----------+----------+");
    lub_heap_context_show_frame(this,frame--);
    printf(      "%10p|    blocks|     bytes|   average|  overhead|",(void*)this);
    lub_heap_context_show_frame(this,frame--);
    printf("+---------+----------+----------+----------+----------+");
    lub_heap_context_show_frame(this,frame--);

    printf("|allocs   |%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|",
            ok_allocs,
            ok_bytes,
            ok_allocs ? (ok_bytes / ok_allocs) : 0,
            ok_allocs*(lub_heap__get_block_overhead() + sizeof(lub_heap_node_t)));
    lub_heap_context_show_frame(this,frame--);

    printf("|partials |%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|",
            this->partials,
            this->partial_bytes,
            this->partials ? (this->partial_bytes/this->partials) : 0,
            this->partials*(lub_heap__get_block_overhead() + sizeof(lub_heap_node_t)));
    lub_heap_context_show_frame(this,frame--);

    printf("|leaks    |%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|%10"SIZE_FMT"|",
            this->leaks,
            this->leaked_bytes,
            this->leaks ? (this->leaked_bytes/this->leaks) : 0,
            this->leaks*(lub_heap__get_block_overhead() + sizeof(lub_heap_node_t)));
    lub_heap_context_show_frame(this,frame--);


    printf("+---------+----------+----------+----------+----------+");
    lub_heap_context_show_frame(this,frame--);

    while(frame >= 0)
    {
        printf("%55s","");
        lub_heap_context_show_frame(this,frame--);
    }

    printf("ALLOCATED BLOCKS: ");
    /* now iterate the allocated nodes */
    lub_heap_context_foreach_node(this,lub_heap_node_show);
    printf("\n\n");
    
}
/*--------------------------------------------------------- */
static void 
lub_heap_context_show_all(lub_heap_context_t *this)
{
    lub_heap_context_show(this,LUB_HEAP_SHOW_ALL);
}
/*--------------------------------------------------------- */
static void 
lub_heap_context_show_partials(lub_heap_context_t *this)
{
    lub_heap_context_show(this,LUB_HEAP_SHOW_PARTIALS);
}
/*--------------------------------------------------------- */
static void 
lub_heap_context_show_leaks(lub_heap_context_t *this)
{
    lub_heap_context_show(this,LUB_HEAP_SHOW_LEAKS);
}
/*--------------------------------------------------------- */
void
lub_heap_context_clear(lub_heap_context_t *this)
{
    /* reset the counters */
    this->allocs        = 0;
    this->alloc_bytes   = 0;
    this->leaks         = 0;
    this->leaked_bytes  = 0;
    this->partials      = 0;
    this->partial_bytes = 0;
    
    /* now clear the referenced nodes */
    while(this->first_node)
    {
        lub_heap_node_clear(this->first_node);
    }
}
/*--------------------------------------------------------- */
void
lub_heap_show_leaks(lub_heap_show_e how)
{
    if(0 < lub_heap_frame_count)
    {
        bool_t bad_arg = BOOL_FALSE;
        
        lub_heap_leak_mutex_lock();
        
        /* check for memory leaks */
        lub_heap_scan_all();
    
        switch(how)
        {
            case LUB_HEAP_SHOW_LEAKS:
            {
                lub_heap_foreach_context(lub_heap_context_show_leaks);
                break;
            }
            case LUB_HEAP_SHOW_PARTIALS:
            {
                lub_heap_foreach_context(lub_heap_context_show_partials);
                break;
            }
            case LUB_HEAP_SHOW_ALL:
            {
                lub_heap_foreach_context(lub_heap_context_show_all);
                break;
            }
            default:
            {
                printf("Invalid argument: 0=leaks, 1=partials and leaks, 2=all allocations\n\n");
                bad_arg = BOOL_TRUE;
                break;
            }
        }
        if(BOOL_FALSE == bad_arg)
        {
            printf("  '*' = leaked memory\n"
                   "  '+' = partially leaked memory\n");
            lub_heap_show_summary();
            printf("  %lu stack frames held for each allocation.\n\n",lub_heap_frame_count);
        }
        lub_heap_leak_mutex_unlock();
    }
    else
    {
        printf("Leak detection currently disabled\n\n");
    }
}
/*--------------------------------------------------------- */
static void
lub_heap_exit_handler(void)
{
    if(0 != lub_heap_frame_count)
    {
        /* dump all currently allocated blocks */
        lub_heap_show_leaks(LUB_HEAP_SHOW_ALL);
    }
}
/*--------------------------------------------------------- */

/*--------------------------------------------------------- 
 * PUBLIC META FUNCTIONS
 *--------------------------------------------------------- */
void
lub_heap__set_framecount(unsigned frames)
{
    if(frames == lub_heap_frame_count)
        return;
    
    lub_heap_leak_mutex_lock();
    
    /* iterate around the contexts getting them to delete themselves */
    lub_heap_foreach_context(lub_heap_context_delete);
    
    /* clear the summary stats */
    lub_heap_leak_stats.contexts      = 0;
    lub_heap_leak_stats.allocs        = 0;
    lub_heap_leak_stats.alloc_bytes   = 0;
    lub_heap_leak_stats.leaks         = 0;
    lub_heap_leak_stats.leaked_bytes  = 0;
    lub_heap_leak_stats.partials      = 0;
    lub_heap_leak_stats.partial_bytes = 0;
    lub_heap_leak_stats.scanned       = 0;
    
    if(frames)
    {
        static bool_t registered = BOOL_FALSE;
        
        if(frames <= MAX_BACKTRACE)
        {
            /* change the number of frames held */
            lub_heap_frame_count = frames;
        }
        else
        {
            fprintf(stderr,"--- leak-detection frame count set to a maximum of %d\n",MAX_BACKTRACE);
            lub_heap_frame_count = MAX_BACKTRACE;
        }
        /* we need to taint memory to ensure old pointers don't mask leaks */
        lub_heap_taint(BOOL_TRUE);
        if(BOOL_FALSE == registered)
        {
            registered = BOOL_TRUE;
            atexit(lub_heap_exit_handler);
        }
    }
    else
    {
        /* switch off leak detection */
        lub_heap_frame_count = 0;
    }
    lub_heap_leak_mutex_unlock();
}
/*--------------------------------------------------------- */
unsigned
    lub_heap__get_framecount(void)
{
    return lub_heap_frame_count;
}
/*--------------------------------------------------------- */
size_t
    lub_heap_context__get_instanceSize(void)
{
    return (sizeof(lub_heap_context_t));
}
/*--------------------------------------------------------- */
const lub_heap_context_t *
lub_heap_context_find(const stackframe_t *stack)
{
    lub_heap_context_t    *result;
    lub_heap_context_key_t key = *stack;

    result = lub_bintree_find(&context_tree,&key);

    return result;
}
/*--------------------------------------------------------- */
