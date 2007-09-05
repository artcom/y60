/*
 * shell_word_generator.c
 */
#include <string.h>
 
#include "private.h"
#include "lub/string.h"
#include "lub/argv.h"
/*-------------------------------------------------------- */
void
clish_shell_iterator_init(clish_shell_iterator_t *iter)
{
    iter->last_cmd_local  = NULL;
    iter->last_cmd_global = NULL;
}
/*-------------------------------------------------------- */
const clish_command_t *
clish_shell_find_next_completion(const clish_shell_t    *this,
                                 const char             *line,
                                 clish_shell_iterator_t *iter)
{
    const clish_command_t *result=NULL,*cmd1,*cmd2;
    int                    diff;

    /* ask the local view for it's next command */
    cmd1 = clish_view_find_next_completion(this->view,
                                           iter->last_cmd_local,
                                           line);
    /* ask the global view for it's next command */
    cmd2 = clish_view_find_next_completion(this->global,
                                           iter->last_cmd_global,
                                           line);
    /* compare the two results */
    diff = clish_command_diff(cmd1,cmd2);
    if(diff > 0)
    {
        result = iter->last_cmd_global = cmd2;
    }
    else
    {
        if(0 == diff)
        {
            /* local view may override global command */
            iter->last_cmd_global = cmd2;
        }
        result = iter->last_cmd_local = cmd1;
    }
    return result;
}
/*--------------------------------------------------------- */
static char *
clish_shell_param_generator(const clish_shell_t   *this,
                            const clish_command_t *cmd,
                            const char            *line,
                            unsigned               offset,
                            unsigned               state)
{
    char                *result      = NULL;
    const char          *name        = clish_command__get_name(cmd);
    char                *text        = lub_string_dup(&line[offset]);
    unsigned             index;
    const clish_param_t *param;
    clish_ptype_t       *ptype;
    
    /* get the index of the current parameter */
    index = lub_argv_wordcount(line) - lub_argv_wordcount(name);

    if((0 != index) || (line[offset-1] == ' '))
    {
        if((0 != index) && (text[0] != '\0'))
        {
            /* if there is some text for the parameter then adjust the index */
            index--;
        }
        /* get the parameter definition */
        param = clish_command__get_param(cmd,index);
        if(NULL != param)
        {
            /* get the parameter type */
            ptype = clish_param__get_ptype(param);
            /* pass the parameter completion onto the ptype class */
            result = clish_ptype_word_generator(ptype,text,state);

            if((state == 0) && (NULL == result))
            {
                /* only show an error if this is a required parameter */
                if (NULL == clish_param__get_default(param))
                {
                    tinyrl_crlf(this->tinyrl);
                    /* the extra '1' is because the clish_param_help()
                     * takes a one-based offset and that passed into here
                     * is zero based 
                     */
                    clish_param_help(param,
                                     1 + offset + strlen(tinyrl__get_prompt(this->tinyrl)));
                    /* make sure we reset the line state */
                    tinyrl_crlf(this->tinyrl);
                    tinyrl_reset_line_state(this->tinyrl);
                }
                tinyrl_completion_error_over(this->tinyrl);
            }
        }
        else if((0 == state) && (NULL != clish_command__get_args(cmd)))
        {
            result = lub_string_dup(text);
        }
            
    }
    else if(0 == state)
    {
        /* simply return the command name */
        result = lub_string_dup(clish_command__get_suffix(cmd));
    }
    lub_string_free(text);
    return result;
}
/*--------------------------------------------------------- */
static char *
clish_shell_command_generator(clish_shell_t *this,
                              const char    *line,
                              unsigned       offset,
                              unsigned       state)
{
    char                  *result = NULL;
    const clish_command_t *cmd    = NULL;
    if(0 == state)
    {
        cmd = clish_shell_getfirst_command(this,line);
    }
    else
    {
        cmd = clish_shell_getnext_command(this,line);
    }

    if(NULL != cmd)
    {
        result = lub_string_dup(clish_command__get_suffix(cmd));
    }
    /* keep the compiler happy */
    offset = offset;
    return result;
}
/*--------------------------------------------------------- */
char *
clish_shell_word_generator(clish_shell_t *this,
                           const char    *line,
                           unsigned       offset,
                           unsigned       state)
{
    char                  *result     = NULL;
    const clish_command_t *cmd, *next = NULL;

    /* try and resolve a command which is a prefix of the line */
    cmd  = clish_shell_resolve_command(this,line);
    if(NULL != cmd)
    {
        clish_shell_iterator_t iter;
        /* see whether there is an extended extension */
        clish_shell_iterator_init(&iter);
        next  = clish_shell_find_next_completion(this,
                                                line,
                                                &iter);
    }
    if((NULL != cmd) && (NULL == next))
    {
        /* this needs to be completed as a parameter */
        result = clish_shell_param_generator(this,cmd,line,offset,state);
    }
    else
    {
        /* this needs to be completed as a command */
        result = clish_shell_command_generator(this,line,offset,state);
    }
    if(0 == state)
    {
        /* reset the state from a help perspective */
        this->state = SHELL_STATE_READY;
    }
    return result;
}
/*--------------------------------------------------------- */
