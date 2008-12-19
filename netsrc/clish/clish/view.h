/*
 * view.h
 */
 /**
\ingroup clish
\defgroup clish_view view
@{

\brief This class is a container of commands.

A particular CLI session may contain a number of different views. Each 
view may contain its own specific commands as well as those available at
a global scope.

*/
#ifndef _clish_view_h
#define _clish_view_h

typedef struct clish_view_s clish_view_t;

#include "clish/command.h"

/*=====================================
 * VIEW INTERFACE
 *===================================== */
/*-----------------
 * meta functions
 *----------------- */
clish_view_t *
		clish_view_new(const char *name,
             	  	       const char *prompt);
int
		clish_view_bt_compare(const void *clientnode,
                	              const void *clientkey);
void
		clish_view_bt_getkey(const void        *clientnode,
                	             lub_bintree_key_t *key);
size_t
		clish_view_bt_offset(void);
/*-----------------
 * methods
 *----------------- */
void
		clish_view_delete(clish_view_t *instance);
clish_command_t *
		clish_view_new_command(clish_view_t *instance,
                		       const char   *name,
                     		       const char   *text);
clish_command_t *
		clish_view_find_command(clish_view_t *instance,
                		        const char   *name);
const clish_command_t *
		clish_view_find_next_completion(clish_view_t          *instance,
                		                const clish_command_t *cmd,
                		                const char            *line);
clish_command_t *
		clish_view_resolve_command(clish_view_t *instance,
                		           const char   *line);                    
clish_command_t *
		clish_view_resolve_prefix(clish_view_t *instance,
				          const char   *line);
void
		clish_view_dump(clish_view_t *instance);
/*-----------------
 * attributes 
 *----------------- */
const char *
		clish_view__get_name(const clish_view_t *instance);
void
		clish_view__set_prompt(clish_view_t *instance,
                		       const char   *prompt);
char *
		clish_view__get_prompt(const clish_view_t  *instance,
                               const char          *viewid);

#endif /* _clish_view_h */
/** @} clish_view */
