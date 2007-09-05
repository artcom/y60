/*
 * param.c
 *
 * This file provides the implementation of the "param" class  
 */
#include "private.h"
#include "lub/string.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/*---------------------------------------------------------
 * PRIVATE METHODS
 *--------------------------------------------------------- */
static void
clish_param_init(clish_param_t *this,
                 const char    *name,
                 const char    *text,
                 clish_ptype_t *ptype)
{

    /* initialise the help part */
    this->name  = lub_string_dup(name);
    this->text  = lub_string_dup(text);
    this->ptype = ptype;

    /* set up defaults */
    this->prefix = NULL;
    this->defval = NULL;
}
/*--------------------------------------------------------- */
static void
clish_param_fini(clish_param_t *this)
{
    /* deallocate the memory for this instance */
    lub_string_free(this->prefix);
    this->prefix = NULL;
    lub_string_free(this->defval);
    this->defval = NULL;
    lub_string_free(this->name);
    this->name = NULL;
    lub_string_free(this->text);
    this->text = NULL;
}
/*---------------------------------------------------------
 * PUBLIC META FUNCTIONS
 *--------------------------------------------------------- */
clish_param_t *
clish_param_new(const char    *name,
                const char    *text,
                clish_ptype_t *ptype)
{
    clish_param_t *this = malloc(sizeof(clish_param_t));

    if(this)
    {
        clish_param_init(this,name,text,ptype);
    }
    return this;
}
/*---------------------------------------------------------
 * PUBLIC METHODS
 *--------------------------------------------------------- */
void
clish_param_delete(clish_param_t *this)
{
    clish_param_fini(this);
    free(this);
}
/*---------------------------------------------------------
 * PUBLIC ATTRIBUTES
 *--------------------------------------------------------- */
const char *
clish_param__get_name(const clish_param_t *this)
{
    return this->name;
}
/*--------------------------------------------------------- */
const char *
clish_param__get_text(const clish_param_t *this)
{
    return this->text;
}
/*--------------------------------------------------------- */
const char *
clish_param__get_range(const clish_param_t *this)
{
    return clish_ptype__get_range(this->ptype);
}
/*--------------------------------------------------------- */
clish_ptype_t *
clish_param__get_ptype(const clish_param_t *this)
{
    return this->ptype;
}
/*--------------------------------------------------------- */
void
clish_param__set_prefix(clish_param_t *this,
                        const char    *prefix)
{
    assert(NULL == this->prefix);
    this->prefix = lub_string_dup(prefix);
}
/*--------------------------------------------------------- */
const char *
clish_param__get_prefix(const clish_param_t *this)
{
    return this->prefix;
}
/*--------------------------------------------------------- */
void
clish_param__set_default(clish_param_t *this,
                         const char    *defval)
{
    assert(NULL == this->defval);
    this->defval = lub_string_dup(defval);
}
/*--------------------------------------------------------- */
const char *
clish_param__get_default(const clish_param_t *this)
{
    return this->defval;
}
/*--------------------------------------------------------- */
char *
clish_param_validate(const clish_param_t *this,
                     const char          *text)
{
    return clish_ptype_translate(this->ptype,text);
}
/*--------------------------------------------------------- */
void
clish_param_help(const clish_param_t *this,
                 size_t               offset)
{
    const char *range = clish_ptype__get_range(this->ptype);
    printf("%*c\n",offset,'^');
    printf("%s  %s",
           this->name, 
           clish_ptype__get_text(this->ptype));
    if(NULL != range)
    {
            printf(" (%s)",range);
    }
    printf("\n");
}
/*--------------------------------------------------------- */
