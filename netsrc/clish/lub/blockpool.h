/**
\ingroup lub
\defgroup lub_blockpool blockpool
 @{
 
\brief This interface provides a facility to manage the allocation and
deallocation of fixed sized blocks of memory.

This type of memory manager is very fast and doesn't suffer from
any memory fragmentation problems.

This allocator uses block in the order in which they are freed,
this is significant for some applications where you don't want to
re-use a particular freed block too soon. e.g. hardware timing
limits on re-use.

The client is responsible for providing the memory to be managed.
\author  Graeme McKerrell
\date    Created On      : Fri Jan 23 12:50:18 2004
\version TESTED
*/
/*---------------------------------------------------------------
 * HISTORY
 * 7-Dec-2004		Graeme McKerrell	
 *    Updated to use the "lub" prefix
 * 6-Feb-2004		Graeme McKerrell	
 *    removed init_fn type definition and parameter, the client had
 *    more flexiblity in defining their own initialisation operation with
 *    arguments rather than use a "one-size-fits-all" approach.
 *    Modified blockpool structure to support FIFO block allocation.
 * 23-Jan-2004		Graeme McKerrell	
 *    Initial version
 *---------------------------------------------------------------
 * Copyright (C) 2004 3Com Corporation. All Rights Reserved.
 *--------------------------------------------------------------- */
#ifndef _lub_blockpool_h
#define _lub_blockpool_h
#include <stddef.h>

/****************************************************************
 * TYPE DEFINITIONS
 **************************************************************** */
/**
 * This type represents a "blockpool" instance.
 */
typedef struct lub_blockpool_s lub_blockpool_t;
struct lub_blockpool_s
{
	/* CLIENTS MUSTN'T TOUCH THESE DETAILS */
        lub_blockpool_t *head;
        lub_blockpool_t *tail;
};

/****************************************************************
 * BLOCKPOOL OPERATIONS
 **************************************************************** */
/**
 * This operation initialises an instance of a blockpool.
 *
 *
 * \pre 'blocksize' must be an multiple of 'sizeof(void *)'.
 * (If the client declares a structure for the block this should be handled
 * automatically by the compiler)
 *
 * \post If the size constraint is not met an assert will fire.
 * \post Following initialisation the allocation of memory can be performed.
 */
extern void
		lub_blockpool_init(
			/** 
			 * the "blockpool" instance to initialise.
			 */
			lub_blockpool_t       *blockpool,
			/** 
			 * the memory to be managed.
			 */
                	void                  *memory,
			/**
			 * The size in bytes of each block.
			 */
		        size_t                 blocksize,
			/** The number of blocks to be managed. NB the client is
			 * responsible for ensuring that (blocksize x blockcount)
			 * bytes of memory are available for use.
			 */
		        unsigned               blockcount
		);

/**
 * This operation allocates a "block" of memory from a "blockpool"
 *
 *
 * \pre
 * The blockpool must have been initialised.
 * 
 * \return
 * A pointer to a "block" of memory, or NULL if there is none left for
 * allocation.
 *
 * \post
 * The behaviour is undefined if the "blockpool" is uninitialised.
 */
extern void *
		lub_blockpool_alloc(
			/** 
			 * the "blockpool" instance to invoke this operation upon
			 */
			lub_blockpool_t *blockpool
		);

/**
 * This operation de-allocates a "block" of memory back into a "blockpool"
 *
 * \pre
 * The specified block must have been previously allocated using
 * lub_blockpool_alloc()
 * 
 * \post
 * The de-allocated block become available for subsequent
 * lub_blockpool_alloc() requests.
 */
extern void
		lub_blockpool_free(
			/** 
			 * the "blockpool" instance to invoke this operation upon
			 */
			lub_blockpool_t *blockpool,
			/** 
			 * the "block" to release back to the pool
			 */
		        void            *block
		);

#endif /* _lub_blockpool_h */
/** @} blockpool */

