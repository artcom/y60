/* 
 * heap_show.c
 */
#include <stdio.h>
#include "private.h"

/*--------------------------------------------------------- */
static void
process_free_block(void     *block,
                   unsigned  index,
                   size_t    size,
                   void     *arg)
{
    /* dump the details for this block */
    printf(" %8p[%"SIZE_FMT"]",block,size);
}
/*--------------------------------------------------------- */
static void
process_segment(void     *segment,
                unsigned  index,
                size_t    size,
                void     *arg)
{
    /* dump the details for this segment */
    printf(" %8p[%"SIZE_FMT"]",segment,size);
}
/*--------------------------------------------------------- */
void
lub_heap_show(lub_heap_t *this,
              bool_t      verbose)
{
    static char     *state_names[] = {"DISABLED","ENABLED"};
    lub_heap_stats_t stats;
    
    if(BOOL_FALSE == lub_heap_check_memory(this))
    {
        printf("*** HEAP CORRUPTED!!! ***\n");
    }
    if(BOOL_TRUE == verbose)
    {
        /* dump each free block's details */
        printf("HEAP SEGMENTS:\n");
        lub_heap_foreach_segment(this,process_segment,NULL);
        printf("\n\n");
        printf("FREE BLOCKS:\n");
        lub_heap_foreach_free_block(this,process_free_block,NULL);
        printf("\n\n");
        printf("GLOBAL:\n"
               "  tainting(%s), full checking(%s), leak detection(%s),\n"
               "  native alignment of %d bytes\n\n", 
               state_names[lub_heap_is_tainting()],
               state_names[lub_heap_is_checking()],
               state_names[0 != lub_heap__get_framecount()],
               LUB_HEAP_ALIGN_NATIVE);
        printf("SUMMARY:\n");
    }

    /* get the stats */
    lub_heap__get_stats(this,&stats);
    
    /* dump the statistics for this heap */
    printf("  status      bytes    blocks  avg block  max block   overhead\n");
    printf("  ------ ---------- --------- ---------- ---------- ----------\n");
    printf("current\n");
    printf("    free %10"SIZE_FMT" %9"SIZE_FMT" %10"SIZE_FMT" %10"SIZE_FMT" %10"SIZE_FMT"\n",
           stats.free_bytes,
           stats.free_blocks,
           stats.free_blocks ? stats.free_bytes /stats.free_blocks : 0,
           lub_heap__get_max_free(this),
           stats.free_overhead);
    printf("   alloc %10"SIZE_FMT" %9"SIZE_FMT" %10"SIZE_FMT" %10s %10"SIZE_FMT"\n",
           stats.alloc_bytes,
           stats.alloc_blocks,
           stats.alloc_blocks ? stats.alloc_bytes / stats.alloc_blocks : 0,
           "-",
           stats.alloc_overhead);
    printf("  static %10"SIZE_FMT" %9"SIZE_FMT" %10"SIZE_FMT" %10s %10"SIZE_FMT"\n",
           stats.static_bytes,
           stats.static_blocks,
           stats.static_blocks ? stats.static_bytes / stats.static_blocks : 0,
           "-",
           stats.static_overhead);

    printf("cumulative\n");
    printf("   alloc %10"SIZE_FMT" %9"SIZE_FMT" %10"SIZE_FMT" %10s %10s\n",
           stats.alloc_total_bytes,
           stats.alloc_total_blocks,
           stats.alloc_total_blocks ? stats.alloc_total_bytes / stats.alloc_total_blocks : 0,
           "-",
           "-");
    printf("high-tide\n");
    printf("    free %10"SIZE_FMT" %9"SIZE_FMT" %10"SIZE_FMT" %10"SIZE_FMT" %10"SIZE_FMT"\n",
           stats.free_hightide_bytes,
           stats.free_hightide_blocks,
           stats.free_hightide_blocks ? stats.free_hightide_bytes /stats.free_hightide_blocks : 0,
           stats.free_hightide_largest,
           stats.free_hightide_overhead);
    printf("   alloc %10"SIZE_FMT" %9"SIZE_FMT" %10"SIZE_FMT" %10s %10"SIZE_FMT"\n",
           stats.alloc_hightide_bytes,
           stats.alloc_hightide_blocks,
           stats.alloc_hightide_blocks ? stats.alloc_hightide_bytes / stats.alloc_hightide_blocks : 0,
           "-",
           stats.alloc_hightide_overhead);
    printf("\n\n");
}
