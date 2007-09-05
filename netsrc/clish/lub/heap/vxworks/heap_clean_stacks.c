#include <string.h>

#include <taskLib.h>

#include "../private.h"

#define MAX_TASKS 1024
int id_list[MAX_TASKS];

/*--------------------------------------------------------- */
void
lub_heap_clean_stacks(void)
{
    int num_tasks,i;
    
    /* disable task switching */
    taskLock();
    
    num_tasks = taskIdListGet(id_list,MAX_TASKS);
    
    /* iterate round the tasks in the system */
    for(i = 0;
        i < num_tasks;
        i++)
    {
        TASK_DESC info;
        if(OK == taskInfoGet(id_list[i],&info))
        {
            char    *p     = info.td_pStackBase - info.td_stackHigh;
            unsigned delta = info.td_stackHigh - info.td_stackCurrent;
            /* 
             * if this is the current task then
             * update the stack pointer information
             */
            if(id_list[i] == taskIdSelf())
            {
                delta = (unsigned)&delta - (unsigned)p;
            }
            /* now clean the stack */
            for(;
                delta--;
                p++)
            {
                *p = 0xCC;
            }
        }
    }
    
    /* reenable task switching */
    taskUnlock();
}
/*--------------------------------------------------------- */