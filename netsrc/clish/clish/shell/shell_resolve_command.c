/*
 * shell_resolve_command.c
 */
#include "private.h"

/*--------------------------------------------------------- */
const clish_command_t *
clish_shell_resolve_command(const clish_shell_t *this,
                            const char          *line)
{
	clish_command_t *cmd1,*cmd2;
	
	/* search the current view and global view */
	cmd1 = clish_view_resolve_command(this->view,line);
	cmd2 = clish_view_resolve_command(this->global,line);

	/* choose the longest match */
	return clish_command_choose_longest(cmd1,cmd2);
}
/*----------------------------------------------------------- */
