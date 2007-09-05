#include <stdio.h>
#include <stdlib.h>

#include <symLib.h>
#include <sysSymTbl.h>
#include <taskLib.h>

#include "../private.h"
#include "../context.h"

/*--------------------------------------------------------- */
/* VxWorks version of this doesn't use the program_name */
void
lub_heap_init(const char *program_name)
{
    program_name = program_name;
    
    /* switch on leak detection */
    lub_heap__set_framecount(MAX_BACKTRACE);
}
/*--------------------------------------------------------- */
void
lub_heap_symShow(unsigned address)
{
    int        value;
    char     * name;
    SYM_TYPE   type;
    
    if(OK == symByValueFind(sysSymTbl,address,&name,&value,&type))
    {
        printf(" %s+%d",name,address-value);
        free(name);
    }
    else
    {
        printf("UNKNOWN @ %p",(void*)address);
    }
}
/*--------------------------------------------------------- */
int
leakEnable(unsigned frame_count)
{
    taskLock();
    
    lub_heap__set_framecount(frame_count ? frame_count : MAX_BACKTRACE);

    taskUnlock();
    
    return 0;
}
/*--------------------------------------------------------- */
int
leakDisable(void)
{
    taskLock();

    lub_heap__set_framecount(0);

    taskUnlock();
    
    return 0;
}
/*--------------------------------------------------------- */
int
leakShow(unsigned how)
{
    lub_heap_show_leaks(how);

    return 0;
}
/*--------------------------------------------------------- */
