/*
 * command.h
 */
#include "clish/command.h"

/*---------------------------------------------------------
 * PRIVATE TYPES
 *--------------------------------------------------------- */
struct clish_command_s
{
    lub_bintree_node_t bt_node;
    char           *name;
    char           *text;
    unsigned        paramc;
    clish_param_t **paramv;
    char           *action;
    clish_view_t   *view;
    char           *viewid;
    char           *detail;
    char           *builtin;
    char           *escape_chars;
    clish_param_t  *args;
};
