/*
  * command.c
  *
  * This file provides the implementation of a command definition  
  */
#include "private.h"
#include "clish/variable.h"
#include "lub/bintree.h"
#include "lub/string.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*---------------------------------------------------------
 * PRIVATE METHODS
 *--------------------------------------------------------- */
static void
clish_command_init(clish_command_t *this,
                   const char      *name,
                   const char      *text)
{
    /* initialise the node part */
    this->name = lub_string_dup(name);
    this->text = lub_string_dup(text);
    
    /* Be a good binary tree citizen */
    lub_bintree_node_init(&this->bt_node);

    /* set up defaults */
    this->paramc       = 0;
    this->paramc       = 0;
    this->paramv       = NULL;
    this->viewid       = NULL;
    this->view         = NULL;
    this->action       = NULL;
    this->detail       = NULL;
    this->builtin      = NULL;
    this->escape_chars = NULL;
    this->args         = NULL;
}
/*--------------------------------------------------------- */
static void
clish_command_fini(clish_command_t *this)
{
    unsigned i;
    
    /* finalize each of the parameter instances */
    for(i=0;
        i < this->paramc;
        i++)
    {
        clish_param_delete(this->paramv[i]);
    }
    /* free the parameter vector */
    free(this->paramv);
    lub_string_free(this->viewid);
    this->viewid = NULL;
    lub_string_free(this->action);
    this->action = NULL;
    lub_string_free(this->name);
    this->name = NULL;
    lub_string_free(this->text);
    this->text = NULL;
    lub_string_free(this->detail);
    this->detail = NULL;
    lub_string_free(this->builtin);
    this->builtin = NULL;
    lub_string_free(this->escape_chars);
    this->escape_chars = NULL;

    if(NULL != this->args)
    {
        clish_param_delete(this->args);
        this->args = NULL;
    }
}

/*---------------------------------------------------------
 * PUBLIC META FUNCTIONS
 *--------------------------------------------------------- */
size_t
clish_command_bt_offset(void)
{
    return offsetof(clish_command_t,bt_node);
}
/*--------------------------------------------------------- */
int
clish_command_bt_compare(const void *clientnode,
                         const void *clientkey)
{
    const clish_command_t *this = clientnode;
    const char            *key  = clientkey;

    return lub_string_nocasecmp(this->name,key);
}
/*--------------------------------------------------------- */
void
clish_command_bt_getkey(const void         *clientnode,
                        lub_bintree_key_t *key)
{
    const clish_command_t *this = clientnode;

    /* fill out the opaque key */
    strcpy((char *)key,this->name);
}
/*--------------------------------------------------------- */
clish_command_t *
clish_command_new(const char *name,
                  const char *text)
{
    clish_command_t *this = malloc(sizeof(clish_command_t));
    
    if(this)
    {
        clish_command_init(this,name,text);
    }
    return this;
}
/*---------------------------------------------------------
 * PUBLIC METHODS
 *--------------------------------------------------------- */
void
clish_command_delete(clish_command_t *this)
{
    clish_command_fini(this);
    free(this);
}
/*--------------------------------------------------------- */
void
clish_command_insert_param(clish_command_t *this,
                           clish_param_t   *param)
{
    size_t          new_size = ((this->paramc+1)*sizeof(clish_param_t*));
    clish_param_t **tmp;
    
    /* resize the parameter vector */
    tmp = realloc(this->paramv,new_size);
    if(NULL != tmp)
    {
        this->paramv = tmp;
        /* insert reference to the parameter */
        this->paramv[this->paramc++] = param;
    }
}
/*--------------------------------------------------------- */
/*
 * Fill out the name and prefix for the command dumping 
 * function.
 */
static void
get_strings_for_dump(const clish_param_t *param,
                     const char       **prefix,
                     const char       **name)
{
    const clish_ptype_t *ptype = clish_param__get_ptype(param);
    
    if(NULL == ptype)
    {
        *prefix = clish_param__get_name(param);
        *name   = "";
    }
    else
    {
        *prefix = clish_param__get_prefix(param);
        *name   = clish_param__get_name(param);
    }
}
/*--------------------------------------------------------- */
void
clish_command_help(const clish_command_t *cmd)
{
    const clish_param_t *p;
    unsigned             i,max_name, max_option;
    unsigned             count = 0;
    
    /* iterate the tree of commands */
    max_name = max_option = 0;
    for(i=0;
        (p = clish_command__get_param(cmd,i));
        i++)
    {
        unsigned    width;
        const char *name;
        const char *option;

        get_strings_for_dump(p,&option,&name);
        width = strlen(name);
        if(width > max_name)
        {
            max_name = width;
        }
        if(NULL != option)
        {
            width = strlen(option);
            if(width > max_option)
            {
                max_option = width;
            }
        }
        count++;
    }
    if(NULL != cmd->args)
    {
        unsigned    width;
        const char *dummy,*name;

        get_strings_for_dump(cmd->args,&name,&dummy);
        width = strlen(name) + 4; /* allow for " ..." */
        if(width > max_name)
        {
            max_name = width;
        }
        count++;
    }
    printf("%s ",clish_command__get_name(cmd));
    for(i=0;
        (p = clish_command__get_param(cmd,i));
        i++)
    {
        const char *name;
        const char *option;
        const char *defval = clish_param__get_default(p);

        get_strings_for_dump(p,&option,&name);
        if(NULL != option || NULL != defval)
        {
            printf("[");
        }
        else 
        {
            printf("<");
        }
        if(NULL != option)
        {
            printf("%s%s",option,*name ? " " : "");
        }
        if(NULL != name)
        {
            printf("%s",name);
        }
        if(NULL != option || NULL != defval)
        {
            printf("]");
        }
        else
        {
            printf(">");
        }
        printf(" ");
    }
    if(NULL != cmd->args)
    {
        const char *dummy,*name;
        get_strings_for_dump(cmd->args,&name,&dummy);
        printf("%s ...",name);
    }
    printf("\n %s\n",clish_command__get_text(cmd));
    if(count)
    {
        printf("\n");
    }
    /* iterate around the parameters */
    for(i=0;
        (p = clish_command__get_param(cmd,i));
        i++)
    {
        const char *name;
        const char *option;
        const char *defval = clish_param__get_default(p);
        const char *text   = clish_param__get_text(p);
        const char *range  = clish_param__get_range(p);

        get_strings_for_dump(p,&option,&name);
        printf(" %-*s %-*s   %s",
               max_option,
               option ? option : "",
               max_name,
               name,
               text); 
        if(NULL != range)
        {
            printf(" (%s)",range);
        }
        if(NULL != defval)
        {
            printf(" [%s]",defval);
        }
        printf("\n");
    }
    if(NULL != cmd->args)
    {
        const char *name;
        const char *dummy;
        char       *tmp = NULL;
        const char *text  = clish_param__get_text(cmd->args);

        get_strings_for_dump(cmd->args,&name,&dummy);
        lub_string_cat(&tmp,name);
        lub_string_cat(&tmp," ...");
        printf(" %-*s %-*s   %s\n",
               max_option,
               "",
               max_name,
               tmp,
               text);
        lub_string_free(tmp);
    }
}
/*--------------------------------------------------------- */
clish_command_t *
clish_command_choose_longest(clish_command_t *cmd1,
                             clish_command_t *cmd2)
{
    unsigned len1 = (cmd1 ? strlen(clish_command__get_name(cmd1)) : 0);
    unsigned len2 = (cmd2 ? strlen(clish_command__get_name(cmd2)) : 0);
    
    if(len2 < len1)
    {
        return cmd1;
    }
    else if(len1 < len2)
    {
        return cmd2;
    }
    else
    {
        /* let local view override */
        return cmd1;
    }
}
/*--------------------------------------------------------- */
int
clish_command_diff(const clish_command_t *cmd1,
                   const clish_command_t *cmd2)
{
    if(NULL == cmd1)
    {
        if(NULL != cmd2)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    if(NULL == cmd2)
    {
        return -1;
    }
    return lub_string_nocasecmp(clish_command__get_name(cmd1),
                                clish_command__get_name(cmd2));
}
/*---------------------------------------------------------
 * PUBLIC ATTRIBUTES
 *--------------------------------------------------------- */
const char *
clish_command__get_name(const clish_command_t *this)
{
    return this->name;
}
/*--------------------------------------------------------- */
const char *
clish_command__get_text(const clish_command_t *this)
{
    return this->text;
}
/*--------------------------------------------------------- */
const clish_param_t *
clish_command_next_non_option(const clish_command_t *cmd,
                              unsigned              *index)
{
    const clish_param_t *param;
    
    while( NULL != (param = clish_command__get_param(cmd,(*index)++)) )
    {
        const clish_ptype_t *ptype  = clish_param__get_ptype(param);
        const char        *prefix = clish_param__get_prefix(param);
        
        /* is this an "option" */
        if((NULL == prefix) && (NULL != ptype))
        {
            /* found what we are looking for... */
            break;
        }
    }
    return param;
}
/*--------------------------------------------------------- */
void
clish_command__set_action(clish_command_t *this,
                          const char      *action)
{
    assert(NULL == this->action);
    this->action = lub_string_dup(action);
}
/*--------------------------------------------------------- */
const char *
clish_command__get_detail(const clish_command_t *this)
{
    return this->detail;
}
/*--------------------------------------------------------- */
void
clish_command__set_detail(clish_command_t *this,
                          const char      *detail)
{
    assert(NULL == this->detail);
    this->detail = lub_string_dup(detail);
}
/*--------------------------------------------------------- */
char *
clish_command__get_action(const clish_command_t *this,
                          const char            *viewid,
                          clish_pargv_t         *pargv)
{
    return clish_variable_expand(this->action,viewid,this,pargv);
}
/*--------------------------------------------------------- */
void
clish_command__set_view(clish_command_t *this,
                        clish_view_t    *view)
{
    assert(NULL == this->view);
    this->view = view;
}
/*--------------------------------------------------------- */
clish_view_t *
clish_command__get_view(const clish_command_t *this)
{
    return this->view;
}
/*--------------------------------------------------------- */
void
clish_command__set_viewid(clish_command_t *this,
                          const char    *viewid)
{
    assert(NULL == this->viewid);
    this->viewid = lub_string_dup(viewid);
}
/*--------------------------------------------------------- */
char *
clish_command__get_viewid(const clish_command_t *this,
                          const char            *viewid,
                          clish_pargv_t         *pargv)
{
    return clish_variable_expand(this->viewid,viewid,this,pargv);
}
/*--------------------------------------------------------- */
const clish_param_t *
clish_command__get_param(const clish_command_t *this,
                         unsigned               index)
{
    clish_param_t *result = NULL;

    if(index < this->paramc)
    {
        result = this->paramv[index];
    }
    return result;
}
/*--------------------------------------------------------- */
const clish_param_t *
clish_command_find_option(const clish_command_t *this,
                          const char            *name)
{
    clish_param_t *result = NULL;
    unsigned       i;
    
    for(i=0;
        i < this->paramc;
        i++)
    {
        clish_param_t       *param  = this->paramv[i];
        const char          *prefix;
        const clish_ptype_t *ptype = clish_param__get_ptype(param);

        prefix = ptype ? clish_param__get_prefix(param) : clish_param__get_name(param);

        if((NULL != prefix) && (0 == strcmp(prefix,name)))
        {
            result = param;
            break;
        }
    }
    return result;
}
/*--------------------------------------------------------- */
const char *
clish_command__get_suffix(const clish_command_t *this)
{
    return lub_string_suffix(this->name);
}
/*--------------------------------------------------------- */
void
clish_command__set_builtin(clish_command_t *this,
                           const char      *builtin)
{
    assert(NULL == this->builtin);
    this->builtin = lub_string_dup(builtin);
}
/*--------------------------------------------------------- */
const char *
clish_command__get_builtin(const clish_command_t *this)
{
    return this->builtin;
}
/*--------------------------------------------------------- */
void
clish_command__set_escape_chars(clish_command_t *this,
                                const char      *escape_chars)
{
    assert(NULL == this->escape_chars);
    this->escape_chars = lub_string_dup(escape_chars);
}
/*--------------------------------------------------------- */
const char *
clish_command__get_escape_chars(const clish_command_t *this)
{
    return this->escape_chars;
}
/*--------------------------------------------------------- */
void
clish_command__set_args(clish_command_t *this,
                        clish_param_t   *args)
{
    assert(NULL == this->args);
    this->args = args;
}
/*--------------------------------------------------------- */
const clish_param_t *
clish_command__get_args(const clish_command_t *this)
{
    return this->args;
}
/*--------------------------------------------------------- */
const unsigned
clish_command__get_param_count(const clish_command_t *this)
{
    return this->paramc;
}
/*--------------------------------------------------------- */
