/*
 * param_dump.c
 */
#include "private.h"
#include "lub/dump.h"
/*--------------------------------------------------------- */
void
clish_param_dump(const clish_param_t *this)
{
    lub_dump_printf("param(%p)\n",this);

    lub_dump_indent();
    lub_dump_printf("name   : %s\n",this->name);
    lub_dump_printf("text   : %s\n",this->text);
    lub_dump_printf("ptype  : %s\n",clish_ptype__get_name(this->ptype));
    lub_dump_printf("prefix : %s\n",this->prefix ? this->prefix : "(null)");
    lub_dump_printf("default: %s\n",this->defval ? this->defval : "(null)");
    lub_dump_undent();
}
/*--------------------------------------------------------- */
