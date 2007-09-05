/*
 * view_dump.c
 */
#include "private.h"
#include "lub/dump.h"

/*--------------------------------------------------------- */
void
clish_view_dump(clish_view_t *this)
{
    clish_command_t        *c;	
    lub_bintree_iterator_t iter;

    lub_dump_printf("view(%p)\n",this);
    lub_dump_indent();
    
    c = lub_bintree_findfirst(&this->tree);

    lub_dump_printf("name : %s\n",clish_view__get_name(this));

    /* iterate the tree of commands */
    for(lub_bintree_iterator_init(&iter,&this->tree,c);
        c;
        c=lub_bintree_iterator_next(&iter))
    {
        clish_command_dump(c);
    }
        
    lub_dump_undent();
}
/*--------------------------------------------------------- */
