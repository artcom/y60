/*
 * command_dump.c
 */
#include "private.h"
#include "lub/dump.h"

/*--------------------------------------------------------- */
void
clish_command_dump(const clish_command_t *this)
{
    unsigned i;
    
    lub_dump_printf("command(%p)\n",this);
    lub_dump_indent();
    lub_dump_printf("name        : %s\n",this->name);
    lub_dump_printf("text        : %s\n",this->text);
    lub_dump_printf("action      : %s\n",this->action ? this->action : "(null)");
    lub_dump_printf("paramc      : %d\n",this->paramc);
    lub_dump_printf("detail      : %s\n",this->detail ? this->detail : "(null)");
    lub_dump_printf("builtin     : %s\n",this->builtin ? this->builtin : "(null)");
    
    /* Get each parameter to dump their details */
    for(i=0;
        i<this->paramc;
        i++)
    {
        clish_param_dump(clish_command__get_param(this,i));
    }
    
    lub_dump_undent();
}
/*--------------------------------------------------------- */
