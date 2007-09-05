/*********************** -*- Mode: C -*- ***********************
 * File            : blockpool_free.c
 *---------------------------------------------------------------
 * Description
 * ===========
 * This operation de-allocates a "block" of memory back into a "blockpool"
 *
 * blockpool - the "blockpool" instance to invoke this operation upon
 *
 * PRE-CONDITIONS
 * The specified block must have been previously allocated using
 * util_blockpool_alloc.
 * 
 * POST-CONDITIONS
 * The de-allocated block become available for subsequent
 * util_blockpool_alloc() requests.
 *---------------------------------------------------------------
 * Author          : Graeme McKerrell
 * Created On      : Thu Jan 29 14:08:41 2004
 * Status          : TESTED
 *---------------------------------------------------------------
 * HISTORY
 * 7-Dec-2004		Graeme McKerrell	
 *    updated to use the "lub_" namespace
 * 5-May-2004		Graeme McKerrell	
 *    updates following review
 * 1-Apr-2004		Peter Kennedy	
 *    updated to free the tail as well as the head
 * 6-Feb-2004		Graeme McKerrell	
 *    updated to use FIFO allocation of blocks
 * 29-Jan-2004		Graeme McKerrell	
 *   Initial version
 *---------------------------------------------------------------
 * Copyright (C) 2004 3Com Corporation. All Rights Reserved.
 **************************************************************** */
#include "private.h"

/*--------------------------------------------------------- */
void
lub_blockpool_free(lub_blockpool_t *this,
                   void            *block)
{
        lub_blockpool_t *newfree = block;

        /* simply add this block to the end of the free list */
        newfree->head = NULL;
        newfree->tail = NULL;
        if(NULL != this->tail)
        {
                this->tail->head = newfree;
        }
        else
        {
                /* first entry in the list */
                this->head = newfree;
        }
        /* add to end of list */
        this->tail = newfree;
}
/*--------------------------------------------------------- */
